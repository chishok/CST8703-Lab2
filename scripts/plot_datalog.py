#!/usr/bin/env python3

import argparse
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import os
import time

# global plot style
mpl.rcParams['lines.linewidth'] = 0.8


def get_timestamp():
    """Get formatted time string

    Returns:
        str: Formatted time string
    """
    return time.strftime('%Y-%m-%d_%Hh%Mm%Ss', time.gmtime(time.time()))


def parse_data_file(datafile: str, dims=1, dtype='f8') -> np.ndarray:
    """Load data from a binary data file

    Args:
        datafile (str): filename of binary data

    Returns:
        np.ndarray:
    """

    with open(datafile, 'rb') as fid:
        data_array = np.fromfile(fid, dtype=dtype)
    if dims > 1:
        return data_array.reshape((-1, dims)).T
    else:
        return data_array


def plot_desirability(datafile: str, output: str, username: str, width=7, height=4, dpi=200, limit_desirability=1.0):

    desirability = parse_data_file(datafile, dtype='f8')

    min_desirability = np.min(desirability)
    max_desirability = np.max(desirability)
    stdev_desirability = np.std(desirability)
    mean_desirability = np.average(desirability)

    num_points = desirability.size
    influencers = np.arange(num_points)

    # plot
    fig, ax = plt.subplots(1, 1)

    ax.plot(influencers, desirability)
    ax.set_title('Desirability Progression')
    ax.set_ylabel('Desirability Factor')
    ax.set_xlabel('Influencer Mention')
    ax.grid(True)

    # filename
    fname = os.path.basename(datafile)
    topstr = fname + '\n' + get_timestamp()
    ax.text(0.5, 0.97, topstr, transform=ax.transAxes, fontsize=8,
            verticalalignment='top', horizontalalignment='center')

    # info
    infostr = []
    infostr.extend(['', 'Min'])
    infostr.append('{:.2f}'.format(min_desirability))
    infostr.extend(['', 'Max'])
    infostr.append('{:.2f}'.format(max_desirability))
    infostr.extend(['', 'Mean, Stdev'])
    infostr.append('{:.2f}, {:.2f}'.format(mean_desirability, stdev_desirability))
    if username is not None:
        infostr.extend(['', 'User:', '{}'.format(username)])
    ax.set_ylim((0.0, 1.0))
    # place info text
    infostr = '\n'.join(infostr)
    ax.text(1.03, 0.97, infostr, transform=ax.transAxes, fontsize=8,
            verticalalignment='top')

    # resize figure
    fig.set_size_inches(width, height)
    fig.tight_layout()

    # save
    if output is None:
        output = os.path.splitext(datafile)[0] + '.png'
    fig.savefig(output, bbox_inches='tight', dpi=dpi)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog='plot_desirability.py',
        description='Plot desirability information from binary file'
    )
    parser.add_argument('--datafile',
                        type=str,
                        help='<Required> binary data file containing desirability in format "f8"',
                        required=True)
    parser.add_argument('--output',
                        type=str,
                        help='Image filename for plot',
                        default=None)
    parser.add_argument('--user',
                        type=str,
                        help='Username',
                        default=None)
    args = parser.parse_args()

    plot_desirability(args.datafile, args.output, args.user)
