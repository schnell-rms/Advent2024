import argparse
import sys

def main(arguments):

    parser = argparse.ArgumentParser(description="Advents of Code")
    
    parser.add_argument("-i", type=str, required=False, metavar="Input path", help="The input file", default="input.txt")
    parser.add_argument("-v", required=False, action="store_true", help="Debug mode", default=False)

    args = parser.parse_args(arguments)

    # Read input file
    ids1 = []
    ids2 = []
    with open(args.i) as input_file:
        for line in input_file:
            numbers = [int(x) for x in line.split()]
            ids1.append(numbers[0])
            ids2.append(numbers[1])
        
    ids1.sort()
    ids2.sort()

    first = sum([abs(x1 - x2) for x1,x2 in zip(ids1, ids2)])
    print("First star",first)
    
    second = sum([x1 * ids2.count(x1) for x1 in ids1])
    print("Second star",second)

if __name__ == "__main__":
    main(sys.argv[1:])

