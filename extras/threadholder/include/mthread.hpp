#pragma once

#include "std_header.h"

namespace mammoth{
	class MThreadPool {
	public:
		static bool create_thread(std::string thread_name, void(*func)(void)) {
			if (thread_pool[thread_name] != nullptr) {
				return false;
			}
			functions[thread_name] = func;
			thread_stopflags[thread_name] = false;
			thread_startflags[thread_name] = true;
			thread_pool[thread_name] = new std::thread(procedure, thread_name);
			return true;
		}

		static void stop_thread(std::string thread_name) {
			if (thread_pool[thread_name] == nullptr) {
				return;
			}
			thread_stopflags[thread_name] = true;
		}

		static void join_thread(std::string thread_name) {
			if (thread_pool[thread_name] == nullptr) {
				return;
			}
			thread_pool[thread_name]->join();
		}
	private:
		static std::unordered_map<std::string, std::thread*> thread_pool;
		static std::unordered_map<std::string, void(*)()> functions;
		static std::unordered_map<std::string, bool> thread_stopflags;
		static std::unordered_map<std::string, bool> thread_startflags;
		static void procedure(std::string thread_name) {
			while (!thread_stopflags[thread_name]) {
				functions[thread_name]();
			}
		}
	};
}

std::unordered_map<std::string, std::thread*> mammoth::MThreadPool::thread_pool;
std::unordered_map<std::string, void(*)()> mammoth::MThreadPool::functions;
std::unordered_map<std::string, bool> mammoth::MThreadPool::thread_stopflags;
std::unordered_map<std::string, bool> mammoth::MThreadPool::thread_startflags;
