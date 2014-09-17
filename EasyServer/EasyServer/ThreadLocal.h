#pragma once

class Scheduler;

extern __declspec(thread) int LThreadType;
extern __declspec(thread) Scheduler* LScheduler;
