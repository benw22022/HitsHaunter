
import numpy as np
from particle import Particle, InvalidParticle, ParticleNotFound
import matplotlib.pyplot as plt
import os

def _get_pos_and_energy(uproot_data, particle_type, event_number=None):
    allowed_types = ["ep", "em", "mm",  "mp", "hm", "hp"]
    if particle_type not in allowed_types:
        raise ValueError(f"particle_type must be one of these {allowed_types}")

    kin_suffixes = ["x", "y", "z", "E"]
    
    cut_expression = f"{particle_type}_z >= 0" 
    
    kinematics = uproot_data.arrays([f"{particle_type}_{k}" for k in kin_suffixes], library="np", cut=cut_expression)
    
    if event_number is None:
        return {"x": kinematics[f"{particle_type}_x"], "y": kinematics[f"{particle_type}_y"], "z": kinematics[f"{particle_type}_z"], "E": kinematics[f"{particle_type}_E"]}
    
    return {"x": kinematics[f"{particle_type}_x"][event_number], "y": kinematics[f"{particle_type}_y"][event_number], "z": kinematics[f"{particle_type}_z"][event_number], "E": kinematics[f"{particle_type}_E"][event_number]}


def _get_name_from_pdgc(pdgc):
    try:
        particle = Particle.from_pdgid(pdgc)
        
        particle_name = particle.name
        
        if pdgc < 0 and "+" not in particle_name and "-" not in particle_name:
            particle_name = f"anti-{particle_name}"
        
        return particle_name
        
    except InvalidParticle:
        return f"Unknown PDG Code: {pdgc}"
    except ParticleNotFound:
        return f"Particle not found: {pdgc}"
    
    
def _get_charge_from_pdgc(pdgc):
    try:
        particle = Particle.from_pdgid(pdgc)
        
        particle_charge = particle.charge
        
        return particle_charge
        
    except InvalidParticle:
        return f"Unknown PDG Code: {pdgc}"
    except ParticleNotFound:
        return f"Particle not found: {pdgc}"


def get_name_from_pdgc(pdgc):
    result = []
    try:
        len(pdgc)
        for p in pdgc:
            result.append(_get_name_from_pdgc(p))
    except TypeError:
        return _get_name_from_pdgc(pdgc)

    return np.array(result)


def get_charge_from_pdgc(pdgc):
    result = []
    try:
        len(pdgc)
        for p in pdgc:
            result.append(_get_charge_from_pdgc(p))
    except TypeError:
        return _get_name_from_pdgc(pdgc)
    
    return np.array(result)


def scatter_on_ax(ax, event_data, x_var, y_var, label, color,  alpha=1, marker_size=1):
    
    if len(event_data[x_var]) == 0:
        return None
    
    return ax.scatter(event_data[x_var], event_data[y_var], color=color, marker=",", label=label, alpha=alpha, s=marker_size)


def get_xyx(event, cut):
    
    x = event['x'][cut]
    y = event['y'][cut]
    z = event['z'][cut]

    return {"x": x, "y": y, "z": z}

def plot_event_z_y_x_y(event, output_dir, show=False):
    
    os.makedirs(output_dir, exist_ok=True)
    
    neutrino_pdgc_to_label_dict = {12: r"$\nu_e$",
                               -12: r"$\bar{\nu}_e$",
                               14: r"$\nu_\mu$",
                               -14: r"$\bar{\nu}_\mu$",
                               16: r"$\nu_\tau$",
                               -16: r"$\bar{\nu}_\tau$",
                                }

    fig, ax = plt.subplots(ncols=2, figsize=(18, 6))
    
    neutrino_pdgc = event['nu_pdgc']
    neutrino_energy = event['nu_E']
    target_pdgc = event['target_pdgc']
    event_number = event['fEvent']
    vx = event['vertex_x']
    vy = event['vertex_y']
    vz = event['vertex_z']
    is_cc = bool(event['isCC'])
    is_cc_label = "CC" if is_cc else "NC"
    
    ax[0].scatter(vz, vy, color="black", marker='x', s=1.5, label="primary vertex", zorder=100)
    ax[1].scatter(vx, vy, color="black", marker='x', s=1.5, label="primary vertex", zorder=100)
    
    marker_size = 1
    alpha = 1
    
    # for p in event['pdgc']:
    #     print(p)

    muons = get_xyx(event, np.abs(event['pdgc']) == 13)
    electrons = get_xyx(event, np.abs(event['pdgc']) == 11)
    taus =  get_xyx(event, np.abs(event['pdgc']) == 15)
    
    # gluons = event[np.where(np.abs(event['pdgc']) == 21)]
    # photons = event[np.where(np.abs(event['pdgc']) == 22)]
    charged_hadrons = get_xyx(event, (np.abs(event['pdgc']) > 37) & (get_charge_from_pdgc(event['pdgc']) != 0))
    # neutral_hadrons = event[np.where(np.abs(event['pdgc']) > 37)]
    # charged_hadrons = event[np.where(get_charge_from_pdgc(event['pdgc']) != 0)]
    # neutral_hadrons = event[np.where(get_charge_from_pdgc(event['pdgc']) == 0)]
    
    # scatter_on_ax(ax[0], neutral_hadrons, "z", "y", "neutral hadrons", 'grey', alpha=0.5, marker_size=marker_size)
    scatter_on_ax(ax[0], charged_hadrons, "z", "y", "charged hadrons", 'forestgreen', alpha=alpha, marker_size=marker_size)
    # scatter_on_ax(ax[0], photons, "z", "y", r"$\gamma$", 'yellow', alpha=0.5, marker_size=marker_size)
    # scatter_on_ax(ax[0], gluons, "z", "y", r"$g$", 'orange', alpha=0.5, marker_size=marker_size)
    scatter_on_ax(ax[0], electrons, "z", "y", r"$e^\pm$", 'lightblue', alpha=alpha, marker_size=marker_size)
    scatter_on_ax(ax[0], muons, "z", "y", r"$\mu^\pm$", 'tomato', alpha=alpha, marker_size=marker_size)
    scatter_on_ax(ax[0], taus, "z", "y", r"$\tau^\pm$", 'purple', alpha=alpha, marker_size=marker_size)
    
    # scatter_on_ax(ax[0], neutral_hadrons, "x", "y", "neutral hadrons", 'grey', alpha=0.5, marker_size=marker_size)
    scatter_on_ax(ax[1], charged_hadrons, "x", "y", "charged hadrons", 'forestgreen', alpha=alpha, marker_size=marker_size)
    # scatter_on_ax(ax[0], photons, "x", "y", r"$\gamma$", 'yellow', alpha=0.5, marker_size=marker_size)
    # scatter_on_ax(ax[0], gluons, "x", "y", r"$g$", 'orange', alpha=0.5, marker_size=marker_size)
    scatter_on_ax(ax[1], electrons, "x", "y", r"$e^\pm$", 'lightblue', alpha=alpha, marker_size=marker_size)
    scatter_on_ax(ax[1], muons, "x", "y", r"$\mu^\pm$", 'tomato', alpha=alpha, marker_size=marker_size)
    scatter_on_ax(ax[1], taus, "x", "y", r"$\tau^\pm$", 'purple', alpha=alpha, marker_size=marker_size)
    
    handles, labels = plt.gca().get_legend_handles_labels()
    by_label = dict(zip(labels, handles))
    
    ax[0].set_title(f"{is_cc_label} {neutrino_pdgc_to_label_dict[neutrino_pdgc]} + {get_name_from_pdgc(target_pdgc)}", loc="left")
    ax[0].set_title(f"Neutrino energy = {neutrino_energy:.2f} GeV", loc="right")
    ax[1].set_title(f"Event Number: {event_number}", loc="right")
    
    ax[0].legend(by_label.values(), by_label.keys())# , ncols=2) #, loc='center right', bbox_to_anchor=(1.55, 0.75))
    ax[1].legend(by_label.values(), by_label.keys())# , ncols=2) #, loc='center right', bbox_to_anchor=(1.55, 0.75)) 
    
    ax[0].set_xlabel("z position (mm)")
    ax[0].set_ylabel("y position (mm)")
    ax[0].set_xlim((550, 1600))
    ax[0].set_ylim((-150, 150))
    
    ax[1].set_xlabel("x position (mm)")
    ax[1].set_ylabel("y position (mm)")
    ax[1].set_xlim((-125, 125))
    ax[1].set_ylim((-150, 150))
    
    plt.savefig(f"{output_dir}/event_{event_number}_particle-dist-x-y-z-y.png", dpi=300, bbox_inches='tight')
    if show:
        plt.show()
    plt.close()
    # break
        