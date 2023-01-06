"""
Usage:
    jsonize_btm2_maps.py <infiles>...
"""

import docopt
import msgpack
import pathlib
import json

args = docopt.docopt(__doc__)


for infile in args["<infiles>"]:
    infile = pathlib.Path(infile).resolve()
    print("Converting", infile.name)
    data = msgpack.load(infile.open("rb"))
    infile.rename(infile.with_suffix(".btm2~"))
    json.dump(data, infile.open("w"))
