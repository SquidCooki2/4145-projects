#pragma once
#include <cstddef>
#include <functional>
#include <omp.h>

class OmpLoop {
  int    nb_threads_ = 1;
  size_t granularity_ = 1;

public:
  void setNbThread(int n)       { nb_threads_  = n; }
  void setGranularity(size_t g) { granularity_ = g; }

  template<typename F>
  void parfor(size_t begin, size_t end, F&& body) const {
    #pragma omp parallel for schedule(dynamic, granularity_) num_threads(nb_threads_)
    for (size_t i = begin; i < end; ++i)
      body(i);
  }
};