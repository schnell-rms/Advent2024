import argparse
import sys


def isSafe(numbers):
    prevSign = 1 if (numbers[1] - numbers[0] > 0) else -1
    for i in range(1,len(numbers)) :
        diff = numbers[i] - numbers[i-1]
        sign = 1 if (diff > 0) else -1
        if ((abs(diff) < 1) or (abs(diff) > 3) or (sign != prevSign)):
            return False
    return True

def isValid(numbers):
    for i in range(len(numbers) + 1):
        num = numbers[:i] + numbers[i+1:]
        if isSafe(num):
            return True
    return False

def main(arguments):

    parser = argparse.ArgumentParser(description="Advents of Code")
    
    parser.add_argument("-i", type=str, required=False, metavar="Input path", help="The input file", default="input.txt")
    parser.add_argument("-v", required=False, action="store_true", help="Debug mode", default=False)

    args = parser.parse_args(arguments)

    # Read input file
    allNumbers = []
    with open(args.i) as input_file:
        for line in input_file:
            allNumbers.append([int(x) for x in line.split()])

    first = [isSafe(report) for report in allNumbers].count(True)
    print("First star",first)

    second = [isValid(report) for report in allNumbers].count(True)
    print("Second star",second)

if __name__ == "__main__":
    main(sys.argv[1:])

