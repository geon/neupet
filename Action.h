#ifndef ACTION
#define ACTION

enum Action {
	move   = 1 << 0,
	battle = 1 << 1,
	mate   = 1 << 2,
	numActions = 3
};

#endif