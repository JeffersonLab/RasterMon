#!/bin/env python
import ROOT
ROOT.gSystem.Load("/data/CLAS12/RasterMon/cmake-build-debug/libRasterLib.dylib")
ras = ROOT.RasterEvioTool("/data/CLAS12/data/evio/clas_pin_016048.evio.00001")
found_tags = []
found_tags_with_event_number = []
tag_counts = {}
ras.tags[0] = 128
ras.tag_masks[0] = 128

while(ras.Next() == 0):
    if( ras.this_tag not in found_tags):
        found_tags.append(ras.this_tag)
        tag_counts[ras.this_tag] = 0
        if( ras.GetEventNumber() > 0 ):
            found_tags_with_event_number.append(ras.this_tag)
    tag_counts[ras.this_tag] += 1

print("tags found: ", found_tags)
print("tags evt run number:", found_tags_with_event_number)
for k in tag_counts.keys():
    print(f"[{k}] = {tag_counts[k]:9d}")
