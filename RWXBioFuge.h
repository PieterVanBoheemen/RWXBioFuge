// set Machine states
enum MachineState {
	StateProgramming,
	StateLock,
	StateRampup,
	StateSpinSteady,
	StateRampdown,
	StateUnlock,
        StatePanic
};

#include "LiquidCrystal_I2C.h"

extern LiquidCrystal_I2C lcd;
