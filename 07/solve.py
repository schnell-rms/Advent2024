import argparse
import re
import sys

def isCalculationOK(numbers, targetSum, isConcatenationAllowed, idx):

    if (idx == 1):
        return targetSum == numbers[1]

    # Go backwards: fail faster. Check all operators
    # Multiplication first
    if targetSum % numbers[idx] == 0 :
        if (isCalculationOK(numbers, targetSum / numbers[idx], isConcatenationAllowed, idx - 1)):
            return True

    if (isConcatenationAllowed):
        s = targetSum
        # Check last digits of s. Would a concatenation do it?
        n = numbers[idx]
        while (n > 0):
            if ( n % 10 == s % 10):
                s //= 10
                n //= 10
            else:
                break

        if ((n == 0) and (isCalculationOK(numbers, s, isConcatenationAllowed, idx - 1))):
            return True

    # Addition last:
    return isCalculationOK(numbers, targetSum - numbers[idx], isConcatenationAllowed, idx - 1)


def checkEquation(numbers, isConcatenationAllowed):
    return numbers[0] if isCalculationOK(numbers, numbers[0], isConcatenationAllowed, len(numbers) - 1) else 0

def main(arguments):

    parser = argparse.ArgumentParser(description="Advents of Code")
    
    parser.add_argument("-i", type=str, required=False, metavar="Input path", help="The input file", default="input.txt")
    parser.add_argument("-v", required=False, action="store_true", help="Debug mode", default=False)

    args = parser.parse_args(arguments)

    sumFirst = 0
    sumConcatenation = 0
    # Read input file
    with open(args.i) as input_file:
        for line in input_file:
            numbers = [int(x) for x in line.replace(":","").split()]
            sumFirst += checkEquation(numbers, False)
            sumConcatenation += checkEquation(numbers, True)


    print("Sum is", sumFirst)
    print("Sum with concatenation is", sumConcatenation)

if __name__ == "__main__":
    main(sys.argv[1:])
