#include "./max_of_vector_elements.h"
#include <boost/mpi/collectives.hpp>
#include <boost/mpi/communicator.hpp>
#include <vector>
#include <random>

std::vector<int> get_random_vector(size_t size, int min_elem, int max_elem){
    std::random_device dev;
    std::mt19937 gen(dev());
    std::uniform_int_distribution<int> distrib(min_elem, max_elem);
    std::vector<int> vec(size);
    for (size_t  i = 0; i < size; i++) 
        vec[i] = distrib(gen); 
    return vec;
}

int get_max_element(const std::vector<int>& init_vec, size_t vec_size){
    boost::mpi::communicator world;
    const int local_vec_size = vec_size / world.size();
    if(world.rank() == 0){
        for(int proc = 1; proc < world.size(); proc++){
            world.send(proc, 0, init_vec.data() + local_vec_size * proc, local_vec_size);
        }
    }

    std::vector<int> local_vec(local_vec_size);
    if(world.rank() == 0){
        local_vec = std::vector<int>(init_vec.begin(), init_vec.begin() + local_vec_size);
    }
    else{
        world.recv(0, 0, local_vec.data(), local_vec_size);
    }

    auto local_res = std::max_element(local_vec.begin(), local_vec.end());
    int global_res;
    reduce(world, *local_res, global_res, boost::mpi::maximum<int>(), 0);
    
    return global_res;
}