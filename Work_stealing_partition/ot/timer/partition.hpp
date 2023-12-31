
#ifndef OT_TIMER_PARTITION_HPP_
#define OT_TIMER_PARTITION_HPP_

#include <ot/timer/wsq.hpp>
#include <ot/timer/vivek.hpp>

namespace ot {

class Timer;
class VivekDAG;
class Partition;  

class Partition {

  friend class Timer;
  friend class VivekDAG;

  public:
    Partition(size_t N); 

  private:
    std::atomic<bool> finish{false}; // check if vivekDAG traversal is finished
    size_t _num_threads;
    std::vector<std::thread> _threads;
    std::vector<WorkStealingQueue<int>> _queues; // work stealing queue to store task id in vivekDAG 
};

// ==========================================================================
//
// Definition of class Partition 
//
// ==========================================================================

inline
Partition::Partition(size_t N): _num_threads(N) {
  for(size_t i=0; i<_num_threads; i++) {
    _threads.emplace_back([this, i, &_vivekDAG](){
      while(!finish) {
        // check if there is any task left in its own queue
        std::optional<int> task_id;
        while(!_queues[i].empty()) {
          task_id = _queues[i].pop();
          if(task_id.has_value()) {  
            // do the task
            VivekTask* vtask = _vivekDAG.vtask_ptrs[task_id.value()];
          }
        }
        // if its own queue is empty, steal task from other queues 
        for(size_t j=0; j<_num_threads; j++) {
          if(j == i) {
            continue;
          }
          task_id = _queues[i].steal();
          if(task_id.has_value()) {
            break;
          }
        }
        // do the task once steal sucessfully
      }
    });
  }
}

}

#endif
