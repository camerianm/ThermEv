# ThermEv

  A C code to explore aspects of planetary thermal evolution.

  To compile using the GNU Compiler Collection:
  gcc -lm -o thermev main.c

  To run a single scenario:
  ./thermev [forward in time? yes=1, no=0] [Urey Ratio, H(t)/Q(t)] [Beta, usually ~0.3]
  Example: thermev 0 0.72 0.30 runs back in time, assuming a Urey ratio of 0.72 and B=0.30.
  Output: summary.dat, containing headers for all outputs; timetemp_[0|1]_[Urey]_[Beta].dat

  To explore a range of scenarios:
  Open stageparams.c in your text editor.
  Check and/or update min/max/increment for Urey ratio and Beta.
  Compile parameter staging: gcc -lm -o stageparams stageparams.c
  Run parameter staging: ./stageparams
  Make staged workflow executable: chmod +x runme.sh
  Run staged workflo: ./runme.sh
  Output: summary.dat in ThermEv directory; all output files in SCENARIOS directory.

  Coming soon:
   - adjustment of relative heat contributions from radionuclides, so forward model can start with early Earth conditions
   - option to nondimensionalize outputs for comparison to 2-D models
   - ability to adjust planetary mass (-> heat capacity), radius (-> surface heat flow), composition (radionuclides, bulk thermal properties)
   - bash scripts to filter output for a given beta or Urey ratio and automate plotting

