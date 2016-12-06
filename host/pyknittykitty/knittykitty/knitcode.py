import re, sys

class KnitOp(object):
    def __init__(self, bed_size):
        self.bed_size = bed_size
        self.left_bed = 0
        self.right_bed = bed_size -1

        self.clear_rows()

    def clear_rows(self):
        self.lbuffer = [False] * self.bed_size
        self.rbuffer = [False] * self.bed_size

    def bytes(self):
        lpacked = []

        byte_count = (self.bed_size / 8) + (1 if self.bed_size % 8 > 0 else 0)
        blist_l = [0] * byte_count
        blist_r = [0] * byte_count

        for i, _ in enumerate(self.lbuffer):
            bit_no  = i % 8
            byte_no = i / 8

            blist_l[byte_no] |= (1 if self.lbuffer[i] else 0) << bit_no
            blist_r[byte_no] |= (1 if self.rbuffer[i] else 0) << bit_no

        return bytearray([self.knitcode, self.left_bed, self.right_bed] + blist_l + blist_r)



class LaceKnit(KnitOp):
    """docstring for LaceKnit."""
    def __init__(self, bed_size):
        super(LaceKnit, self).__init__(bed_size)
        self.knitcode = 7

    def compile(self, code, start=0, end=-1):
        if end < 0:
            end = self.bed_size-1

        if end > self.bed_size-1:
            end = self.bed_size-1

        if start < 0:
            start = 0

        if start > self.bed_size-1:
            return (["Start of operation was greater than bed size, no output"], [])

        if start >= end:
            return (["Start position >= end position, no output"], [])

        self.clear_rows()
        self.left_bed = start
        self.right_bed = end

        # Break the input code up over the operations
        code = "".join(code.split())
        ops = code.split(",")

        errors = []
        warnings = []
        position = start

        for op in ops:
            num = 1

            operation = "".join(re.split("[0-9]", op))

            # Check for valid post-fix multiple
            if len(operation) < len(op):
                try:
                    num = int(op[len(operation):])
                except ValueError:
                    errors += ["The value \"%s\" is not a valid post-fix" % op[len(operation):]]
                    break
                if num < 1:
                    errors += ["The post fix value must be greater than 0!"]

            # Check for valid argument
            if operation not in ["LS", "RS", "N", "LRS", "RLS"]:
                errors += ["Operation \"%s\" unknown!" % operation]

            buff = self.lbuffer
            value = False
            if operation == "LS":
                for _ in xrange(num):
                    self.rbuffer[position] = True
                    position += 1
            elif operation == "RS":
                for _ in xrange(num):
                    self.lbuffer[position] = True
                    position += 1

            elif operation == "LRS" or operation == "RLS":
                for _ in xrange(num):
                    self.lbuffer[position] = True
                    self.rbuffer[position] = True
                    position += 1

            elif operation == "N":
                for _ in xrange(num):
                    self.lbuffer[position] = False
                    self.rbuffer[position] = False
                    position += 1

        if position < end and len(errors) == 0:
            warnings += ["Operation didn't fill out entire row!"]

        return (warnings, errors)


class Knit2(KnitOp):
    """docstring for Knit2."""
    def __init__(self, bed_size):
        super(Knit2, self).__init__(bed_size)
        self.knitcode = 6

    def compile(self, code, start=0, end=-1):
        if end < 0:
            end = self.bed_size-1

        if end > self.bed_size-1:
            end = self.bed_size-1

        if start < 0:
            start = 0

        if start > self.bed_size-1:
            return (["Start of operation was greater than bed size, no output"], [])

        if start >= end:
            return (["Start position >= end position, no output"], [])

        self.clear_rows()
        self.left_bed = start
        self.right_bed = end

        # Break the input code up over the operations
        code = "".join(code.split())
        ops = code.split(",")

        errors = []
        warnings = []
        position = start

        for op in ops:
            num = 1

            operation = "".join(re.split("[0-9]", op))

            # Check for valid post-fix multiple
            if len(operation) < len(op):
                try:
                    num = int(op[len(operation):])
                except ValueError:
                    errors += ["The value \"%s\" is not a valid post-fix" % op[len(operation):]]
                    break
                if num < 1:
                    errors += ["The post fix value must be greater than 0!"]


            # Check for valid post-fix multiple
            if operation not in ["K", "KK", "KC", "CK", "CC", "C"]:
                errors += ["Operation \"%s\" unknown!" % operation]

            if operation == "KK" or operation == "K":
                for _ in xrange(num):
                    self.lbuffer[position] = False
                    self.rbuffer[position] = False
                    position += 1

            elif operation == "KC":
                for _ in xrange(num):
                    self.lbuffer[position] = False
                    self.rbuffer[position] = True
                    position += 1

            elif operation == "CF":
                for _ in xrange(num):
                    self.lbuffer[position] = True
                    self.rbuffer[position] = False
                    position += 1

            elif operation == "C" or operation == "CC":
                for _ in xrange(num):
                    self.lbuffer[position] = True
                    self.rbuffer[position] = True
                    position += 1
            else:
                errors += ["Unknown operation \"%s\"" % operation]

        if position < end and len(errors) == 0:
            warnings += ["Operation didn't fill out entire row!"]

        return (warnings, errors)


def compile_knitcode(code):
    lines = re.split("(\r\n)|(\n)",code)[::3]
    line_no = 0
    lines_joined = []

    while line_no < len(lines):
        ended = False
        c_line = ""
        l_start = line_no
        while not ended:
            t_line   = "".join(lines[line_no].split())
            c_line  += " " + t_line
            line_no += 1

            if len(t_line) == 0:
                break
            if t_line[-1] == ';':
                break

        if len("".join(c_line.split())) > 0:
            lines_joined += [(l_start, c_line)]

    errors = []
    warnings = []
    compiled = []

    for line_no, line in lines_joined:
        ls = line.split('(')
        operation = "".join(ls[0].strip())
        rest = "".join(ls[1:])[:-1]

        # Validate operation
        if operation not in ["K","L"]:
             errors += ["Invalid operation '%s'" % operation]
             continue

        # Try to pull out the range
        try:
            krange = rest.split(')')[0]
            code = rest.split(')')[1]
        except IndexError:
            errors += ["Malformed instruction '%s'" % line]
            continue

        # Try to parse the range
        try:
            start = int(krange.split(':')[0])
            end = int(krange.split(':')[1])
        except IndexError, ValueError:
            errors += ["Malformed range '%s'" % krange]
            continue


        if operation == "K":
            bc = Knit2(200)
            lwarn, lerr = bc.compile(code, start, end)

        elif operation == "L":
            bc = LaceKnit(200)
            lwarn, lerr = bc.compile(code, start, end)

        errors   += lerr
        warnings += lwarn

        if len(lerr) == 0:
            compiled += [(line_no, bc)]

    return compiled, warnings, errors

if __name__ == "__main__":
    kc_file = open(sys.argv[1])
    output = open("k.out", 'wb')
    content = kc_file.readlines()

    code, warnings, errors = compile_knitcode(''.join(content))

    if len(errors) > 0:
        print errors
    print warnings

    for _, o in code:
        output.write(o.bytes())

    output.close()
