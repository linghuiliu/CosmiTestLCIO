#!/usr/bin/env python
import os, sys
from optparse import OptionParser

__doc__ = """Create pID tree containing particle ID information from Cherenkov counters for W-HCal test-beam. 
The tree contains boolean variables ('electron', 'muon', 'pion', 'kaon', 'proton') that are true if the particle hypothesis is consistent with the Cherenkov measurement."""

__author__ = "Bruno Lenzi <Bruno.Lenzi@cern.ch>"

# ***************************************
# get_HCalPIDtree.py -h for help
# ***************************************

def readRunInfo(elogfile, run):
  "Return the beam energy and the pressure in the Cherenkov counters, given the Elog file and the run number"
  for line in open(elogfile):
    values = line.split('|')
    if int(values[0]) == run:
      return map(float, values[3:4] + values[8:10])
  raise AssertionError("\n*** ERROR: Run %s not found in Elog file (or wrong Elog file format)\n" % run )

parser = OptionParser()
parser.description = __doc__
parser.epilog = """The user must supply, apart from the input and output files, either the beam energy (where the sign is used as the charge)
and the pressure in the Cherenkov counters *OR*
the run number and the formatted Elog dump file (see calice_db_tools/examples/README)\n"""

parser.usage = "%s <inputfile> <outputfile>" % sys.argv[0]
parser.add_option("-r", "--run", help="Run number", type="int")
parser.add_option("-e", "--elog-file", dest="elog", help="Elog dump file")
parser.add_option("-E", "--energy", help="Beam energy", type="float")
parser.add_option("-p", "--pressure", help="Pressure in Cherenkov counters A and B, separated by comma")
parser.add_option("-t", "--tree", help="Name of the tree in input file [bigtree]", default="bigtree")

(options, args) = parser.parse_args()
try:
  inputfile, outfile = args
except ValueError:
  print "\n*** ERROR: Input and/or output file not given, type %s -h for help\n" % sys.argv[0]
  sys.exit(1)

# Retrieve beam energy and pressures from command line arguments (or Elog file)
if options.pressure and options.energy:
  try:
    pA, pB = map(float, options.pressure.split(','))
  except ValueError:
    print '\n*** ERROR: Wrong format for pressure in Cherenkov counters: should be pA,pB\n'
    sys.exit(1)
elif options.elog is not None and options.run is not None:
  try:
    options.energy, pA, pB = readRunInfo(options.elog, options.run)
  except IOError, e:
    print '\n*** ERROR: Elog file %s not found\n' % options.elog
    sys.exit(1)
  except AssertionError, e:
    print e.args[0]
    sys.exit(1)
  except ValueError, e:
    print "\n*** ERROR: Could not retrieve beam energy and pressure in Cherenkov counters from Elog file, probably wrong format\n"
    sys.exit(1)
else:
  print '\n*** ERROR: values not supplied. %s' % parser.epilog
  sys.exit(1)

print 'Beam energy: %f / pressures: %.3f,%.3f' % (options.energy, pA, pB)
#sys.exit(1)

############################################################
import ROOT
from array import array

particles = ['electron', 'muon', 'pion', 'kaon', 'proton']
masses = [0.5e-3, 0.105, 0.140, 0.494, 0.938] # e, mu, pi, K, p masses in GeV

def gamma2(P, m):
  "Returns the square of the Lorentz factor gamma, given the momentum and the mass of the particle"
  return 1 + (P/m)**2

def cherenkovThreshold(P, m):
  """cherenkovThreshold(P, m) -> Return the minimum pressure in CO2 required for a particle with
  momentum P and mass M to produce Cherenkov light"""
  return 1194/gamma2(P, m)


# Get the data tree and prepare to read only the Cherenkov information
f = ROOT.TFile(inputfile)
if not f.IsOpen():
  sys.exit(1)

tree = f.Get(options.tree)
if not tree:
  print 'Tree %s not found in input file' % options.tree
  sys.exit(1)

tree.SetBranchStatus('*', 0)
tree.SetBranchStatus('cherenkowBit', 1)
tree.SetBranchStatus('cherenkow2Bit', 1)


# Calculate the Cherenkov thresholds and the expected pattern for each particle:
# - True if the pressure in the counter is above the threshold and thus a signal is expected
# - False otherwise
thresholds = [cherenkovThreshold(options.energy, m) for m in masses ]
cherenkovPattern = [(pA > th, pB > th) for th in thresholds]

# Open output file and create tree pID with 'electron', 'muon', 'pion', 'kaon' and 'proton' as branches
fout = ROOT.TFile(outfile, 'recreate')
pID = ROOT.TTree('pID', 'pID based on Cherenkov counters for W-HCal test-beam')
arrays = [array('b', [0]) for i in particles]
for p,a in zip(particles, arrays):
  pID.Branch(p, a, 'p/b')

# Fill pID tree with the particle hypothesis:
# if the signal in each counter matches the expected pattern, the particle is accepted
for entry in xrange(tree.GetEntries()):
  tmp = tree.GetEntry(entry)
  for a, cP in zip(arrays, cherenkovPattern):
    if a is not arrays[-1] or options.energy > 0: # do not consider protons if the beam has particles with negative charge
      a[0] = (tree.cherenkowBit, tree.cherenkow2Bit) == cP # test particle hypothesis and store the result in the corresponding array
  
  tmp = pID.Fill()

pID.Write()
fout.Close()
