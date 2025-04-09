import ROOT
import os
import argparse

def main(args):
    
    df = ROOT.RDataFrame(args.tree, args.input)

    filters = []
    
    for pdgc in args.has_hit:
        df = df.Define(f"has_{abs(pdgc)}", f"std::find(pdgc.begin(), pdgc.end(), {abs(pdgc)}) != pdgc.end() || std::find(pdgc.begin(), pdgc.end(), -{abs(pdgc)}) != pdgc.end()")
        filters.append(f"has_{abs(pdgc)}")
    
    for pdgc in args.keep_hit:
        df = df.Define("x", f"x[abs(pdgc) == {abs(pdgc)}]")
        df = df.Define("y", f"y[abs(pdgc) == {abs(pdgc)}]")
        df = df.Define("z", f"z[abs(pdgc) == {abs(pdgc)}]")
        df = df.Define("E", f"E[abs(pdgc) == {abs(pdgc)}]")
        df = df.Define("pdgc", f"pdgc[abs(pdgc) == {abs(pdgc)}]")
        df = df.Define("charge", f"charge[abs(pdgc) == {abs(pdgc)}]")
        df = df.Define("layer", f"layer[abs(pdgc) == {abs(pdgc)}]")
    
    cc_el_filter = "abs(nu_pdgc) == 12 && isCC == 1"
    cc_mu_filter = "abs(nu_pdgc) == 14 && isCC == 1"
    cc_ta_filter = "abs(nu_pdgc) == 16 && isCC == 1"
    nc_filter = "isCC == 0"
    
    if "CCe" in args.interaction: filters.append(cc_el_filter)
    if "CCmu" in args.interaction: filters.append(cc_mu_filter)
    if "CCtau" in args.interaction: filters.append(cc_ta_filter)
    if "NC" in args.interaction: filters.append(nc_filter)
    
    # Combine all filters into a single string
    combined_filter = " && ".join(filters)
    skimmed_df = df.Filter(combined_filter)

    # Configure number of events
    if args.nevents > 0:
        skimmed_df = skimmed_df.Range(args.nevents)
    
    # Save the filtered data
    skimmed_df.Snapshot("Hits", args.output)
    
if __name__ == "__main__":
    
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", "-i", help='input file', type=str)
    parser.add_argument("--output", "-o", help='output file', type=str)
    parser.add_argument("--tree", "-t", help='tree name', type=str, default="Hits")
    parser.add_argument("--interaction", "-c", nargs="+", help='Interaction type', type=str, default=[], options=["CCe", "CCmu", "CCtau", "NC"])
    parser.add_argument("--has_hit", "-p", nargs="+", help='Select events only containing a hit with pdg code', type=int, default=[])
    parser.add_argument("--keep_hit", "-k", nargs="+", help='Select events only containing a hit with pdg code', type=int, default=[])
    parser.add_argument("--nevents", "-n", help='Keep only the 1st n events', type=int, default=0)
    args = parser.parse_args()
    
    main(args)