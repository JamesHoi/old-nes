#############################################################
# CA65 labels.txt to fceux's .nl files converter script
# Original script by rainwarrior.
# Edited for multiple banks compatibility by Nesrocks 2018
# Visit nesrocks.com
# version 1.3
#############################################################


# All labels that have a bk<number>_ prefix will go to the appropriate file.
# For example "bk7_reset_handler:" will go to the gamename.nes.7.nl file.
#
# Labels that have no prefix in this format and have address 0x8000+ will go to bank 0 file
# and may be randomly overwritten if the memory address is the same.
# Ram and zeropage labels do not need prefixes, they are sorted by the address.
#
# Warning: this script will delete .nl files with the same rom name that weren't freshly created.
# Script best used in a batch sequence on game compile.
#
# Original folder structure:
# compile.bat
# src/main.asm
# src/system/example_fceux_symbols.py
#
# batch command calling this script:
# cd src
# system\example_fceux_symbols.py
#
# You may need to edit this script to adjust for paths in your project folder structure.


#################
# User parameters
#################

banks = 8 # Set your bank count here. i.e: 8 = banks 0-7. If set to 2 it will go into "old" mode and generate files based on addresses only (ram.nl, 0.nl and 1.nl)
trim_prefix = 1 # 0-1. Option to remove the label prefixes on the converted debug file. can help readability on fceux's debugger.
romname = "old" # Write your ROM name here, minus .nes extension. No special characters.

#################
# User params end
#################

import sys
import os
assert sys.version_info[0] >= 3, "Python 3 required."

from collections import OrderedDict


def label_to_nl(label_file):
    rawlabels = []

    try:
        of = open(label_file, "rt")
        rawlabels = of.readlines()
    except IOError:
        print("skipped: "+label_file)
        return

    labels = [] # a list of lists. finally will contain a list of labels for each bank. will overwrite labels with same address on the same bank
    ramlabels = [] # zeropage and ram labels go here
    ramstrout = ""
    str_outs = []
    nl_files = []
    prefixes = []

    for bank in range(banks):
        str_outs.append("")
        nl_files.append("rom/"+romname + ".nes." + str(bank) + ".nl")
        prefixes.append("bk" + str(bank) + "_")
        labels.append([])

    # read the raw labels string and populate the labels list of lists
    for line in rawlabels:

        # get a list of words, splitting on spaces
        words = line.split()

        if (words[0] == "al"):
            address = int(words[1], 16) # the memory address
            label = words[2] # the actual symbol
            label = label.lstrip('.') # remove everything to the left of ".", including it

            if (label[0] == '_' and label[1] == '_'): # if the first and second letters of the symbol are _ _
                continue # skip compiler internals


            hasbank = 0 # flag to know if the label on this line has a prefix. if it doesn't, throw it into bank 0

            if (address >= 0x0000 and address <= 0x7FF):
                ramlabels.append(label)
                ramstrout += ("$%04X#%s#\n" % (address, label))

            else:
                for bank in range(banks):
                    if label.startswith(prefixes[bank]):
                        if trim_prefix:
                            label = label[4:]
                        labels[bank].append(label)
                        str_outs[bank] += ("$%04X#%s#\n" % (address, label))
                        hasbank = 1 # set the flag so it skips the next part

                # no prefix detected flag
                if hasbank == 0:
                    # throw into bank 0 if generic amount of banks or if address is bank 0
                    if banks != 2 or (address >= 0x8000 and address <= 0xBFFF):
                        labels[0].append(label)
                        str_outs[0] += ("$%04X#%s#\n" % (address, label))
                    # address must be bank 1
                    elif (address >= 0xC000 and address <= 0xFFFF):
                        labels[1].append(label)
                        str_outs[1] += ("$%04X#%s#\n" % (address, label))

    # write each file that isn't empty
    for bank in range(banks):
        if str_outs[bank] != "":
            open(nl_files[bank], "wt").write(str_outs[bank])
            print("debug symbols: " + nl_files[bank])

        # delete old nl files if they exist
        elif os.path.isfile(nl_files[bank]):
            os.remove(nl_files[bank])

    if ramlabels != "":
        open("rom/"+romname + ".nes.ram.nl", "wt").write(ramstrout)
        print("debug symbols: " + romname + ".nes.ram.nl")


if __name__ == "__main__":
    label_to_nl("rom/"+romname + ".labels.txt")