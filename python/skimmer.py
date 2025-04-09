import ROOT
import os
import argparse

def main(args):
    
    df = ROOT.RDataFrame(args.tree, args.input)

    filters = []
    
    for pdgc in args.has_hit:
        print(f"Selecting events with absolute pdg code {pdgc}")
        df = df.Define(f"has_{abs(pdgc)}", f"std::find(pdgc.begin(), pdgc.end(), {abs(pdgc)}) != pdgc.end() || std::find(pdgc.begin(), pdgc.end(), -{abs(pdgc)}) != pdgc.end()")
        filters.append(f"has_{abs(pdgc)}")
    
    for pdgc in args.keep_hit:
        print(f"Keeping hits with absolute pdg code {pdgc}")
        df = df.Define("x", f"x[abs(pdgc) == {abs(pdgc)}]")
        df = df.Define("y", f"y[abs(pdgc) == {abs(pdgc)}]")
        df = df.Define("z", f"z[abs(pdgc) == {abs(pdgc)}]")
        df = df.Define("E", f"E[abs(pdgc) == {abs(pdgc)}]")
        df = df.Define("pdgc", f"pdgc[abs(pdgc) == {abs(pdgc)}]")
        df = df.Define("charge", f"charge[abs(pdgc) == {abs(pdgc)}]")
        df = df.Define("layer", f"layer[abs(pdgc) == {abs(pdgc)}]")
    
    if args.interaction != []:
        cc_el_filter = "abs(nu_pdgc) == 12 && isCC == 1"
        cc_mu_filter = "abs(nu_pdgc) == 14 && isCC == 1"
        cc_ta_filter = "abs(nu_pdgc) == 16 && isCC == 1"
        nc_filter = "isCC == 0"
        
        if "CCe" in args.interaction: 
            print("Selecting CC electron events")
            filters.append(cc_el_filter)
            
        if "CCmu" in args.interaction: 
            filters.append(cc_mu_filter)
            print("Selecting CC muon events")
            
        if "CCtau" in args.interaction: 
            filters.append(cc_ta_filter)
            print("Selecting CC tau events")
            
        if "NC" in args.interaction: 
            filters.append(nc_filter)
            print("Selecting NC events")

    # Combine all filters into a single string
    if len(filters) == 1: df = df.Filter(filters[0])
    elif len(filters) > 1:
        combined_filter = " && ".join(filters)
        df = df.Filter(combined_filter)

    # Configure number of events
    if args.nevents > 0:
        print(f"Keeping only the first {args.nevents} events")
        df = df.Range(args.nevents)
    
    # Save the filtered data
    df.Snapshot("Hits", args.output)
    
if __name__ == "__main__":
    
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", "-i", help='input file', type=str)
    parser.add_argument("--output", "-o", help='output file', type=str)
    parser.add_argument("--tree", "-t", help='tree name', type=str, default="Hits")
    parser.add_argument("--interaction", "-c", nargs="+", help='Interaction type', type=str, default=[], choices=["CCe", "CCmu", "CCtau", "NC"])
    parser.add_argument("--has_hit", "-p", nargs="+", help='Select events only containing a hit with pdg code', type=int, default=[])
    parser.add_argument("--keep_hit", "-k", nargs="+", help='Select events only containing a hit with pdg code', type=int, default=[])
    parser.add_argument("--nevents", "-n", help='Keep only the 1st n events', type=int, default=0)
    args = parser.parse_args()
    
    main(args)