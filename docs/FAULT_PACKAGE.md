# Fault Package

`snooker_fault_package fault.txt shot.input output-directory` creates a standalone
directory with the original fault, valid checkpoint, exact physics bundle, shot
input, concise log, manifest and reproduction instructions. The destination must
be empty so stale evidence cannot be mixed with a new failure.

Run `snooker_fault_replay fault.txt` from inside the package. Tests reproduce the
same code/step using package files only. Invalid numerical candidates remain text
diagnostics in fault-v2; replay always starts from the last valid checkpoint.
