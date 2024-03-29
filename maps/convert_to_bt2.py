"""
Usage:
    convert_to_bt2.py <infiles>...
"""

import docopt
import msgpack
import pathlib

args = docopt.docopt(__doc__)


for infile in args["<infiles>"]:
    objects = []
    tank_side = 1
    infile = pathlib.Path(infile).resolve()
    print("Converting", infile.name)

    with open(infile) as fin:
        for line in fin.read().splitlines():
            type, x, y = map(int, line.split())
            objects.append(
                {
                    "type": type,
                    "x": x * 9.0,
                    "y": y * 9.0,
                    "side": tank_side if type == 3 else 0,
                }
            )
            if type == 3:
                tank_side += 1

    print(" ->", len(objects))

    with open(infile.with_suffix(".btm2"), "wb") as fout:
        msgpack.dump(objects, fout)
