#include <sycl/sycl.hpp>
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>

// Function to perform matrix multiplication using SYCL
void matrix_multiply(sycl::queue &q, size_t N, const std::string &device_name) {
    // Initialize matrices A and B with 1.0 and matrix C with 0.0
    std::vector<float> A(N * N, 1.0f);
    std::vector<float> B(N * N, 1.0f);
    std::vector<float> C(N * N, 0.0f);

    // Create SYCL buffers for matrices A, B, and C
    sycl::buffer<float, 2> bufA(A.data(), sycl::range<2>(N, N));
    sycl::buffer<float, 2> bufB(B.data(), sycl::range<2>(N, N));
    sycl::buffer<float, 2> bufC(C.data(), sycl::range<2>(N, N));

    // Record the start time for performance measurement
    auto start = std::chrono::high_resolution_clock::now();

    // Submit a command group to the queue for execution
    q.submit([&](sycl::handler &h) {
        // Get access to the buffers for reading and writing
        auto a = bufA.get_access<sycl::access::mode::read>(h);
        auto b = bufB.get_access<sycl::access::mode::read>(h);
        auto c = bufC.get_access<sycl::access::mode::write>(h);

        // Define a parallel computation over the matrix dimensions
        h.parallel_for(sycl::range<2>(N, N), [=](sycl::id<2> idx) {
            size_t i = idx[0]; // Row index
            size_t j = idx[1]; // Column index
            float sum = 0.0f;  // Initialize sum for the dot product
            for (size_t k = 0; k < N; ++k) {
                sum += a[i][k] * b[k][j]; // Compute dot product
            }
            c[i][j] = sum; // Store result in matrix C
        });
    }).wait(); // Wait for the computation to finish

    // Record the end time and calculate elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time on " << device_name << ": " << elapsed.count() << " seconds\n";
}

int main(int argc, char *argv[]) {
    // Check if the user provided a device type argument
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <device_type>\n";
        std::cerr << "0: CPU, 1: GPU, 2: CPU & GPU\n";
        return 1;
    }

    int device_type = std::stoi(argv[1]); // Convert argument to integer
    size_t N = 8 * 1024; // Define matrix size (N x N)

    try {
        if (device_type == 2) {
            // Create queues for both CPU and GPU
            sycl::queue cpu_queue(sycl::cpu_selector{});
            sycl::queue gpu_queue(sycl::gpu_selector{}); //how to create the AISC

            std::cout << "Running on CPU and GPU concurrently...\n";

            // Launch matrix multiplication on both devices concurrently using threads
            std::thread cpu_thread(matrix_multiply, std::ref(cpu_queue), N, "CPU");
            std::thread gpu_thread(matrix_multiply, std::ref(gpu_queue), N, "GPU");

            // Wait for both threads to finish
            cpu_thread.join();
            gpu_thread.join();
        } else {
            // Run matrix multiplication on a single specified device
            sycl::device dev = (device_type == 0) ? sycl::device(sycl::cpu_selector{}) : sycl::device(sycl::gpu_selector{});
            sycl::queue q(dev);
            std::cout << "Running on " << dev.get_info<sycl::info::device::name>() << "\n";
            matrix_multiply(q, N, dev.get_info<sycl::info::device::name>());
        }
    } catch (sycl::exception const &e) {
        // Catch and report any SYCL exceptions
        std::cerr << "SYCL exception caught: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
