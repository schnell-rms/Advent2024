import argparse
import re
import sys

def main(arguments):

    parser = argparse.ArgumentParser(description="Advents of Code")
    
    parser.add_argument("-i", type=str, required=False, metavar="Input path", help="The input file", default="input.txt")
    parser.add_argument("-v", required=False, action="store_true", help="Debug mode", default=False)

    args = parser.parse_args(arguments)

    # Read input file
    with open(args.i) as input_file:
        matches = re.findall(r"mul\((\d+),(\d+)\)|(don't\(\))|(do\(\))", input_file.read())

    isActive = True
    first = 0
    second = 0

    # print(matches)
    for p1, p2, donot, do in matches:
        if donot == "don't()":
            isActive = False
        elif do == "do()":
            isActive = True
        else:
            first += int(p1) * int(p2)
            if isActive:
                second += int(p1) * int(p2)

    print("First star",first)
    print("Second star",second)

if __name__ == "__main__":
    main(sys.argv[1:])

