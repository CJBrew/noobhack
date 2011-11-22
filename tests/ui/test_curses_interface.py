import sys
import curses

from flexmock import flexmock

from noobhack.ui.minimap import Minimap
from noobhack.game.mapping import Level, Map

from utils import level_chain

def get_color(_):
    return 0

def fixed_graph():
    levels = level_chain(5, "main")
    dmap = Map(levels[0], 0, 0)
    dmap.levels = set(levels)
    return dmap

def newpad():
    pad = flexmock(None)
    pad.should_receive("addstr")
    return pad

def test_color_doesnt_color_the_ellipsis():
    window = newpad()
    window.should_receive("chgat").times(3)

    m = Minimap()
    dungeon = fixed_graph()
    dungeon.current = [l for l in dungeon.levels if l.dlvl == 5][0]
    m.draw_dungeon(dungeon, window, get_color)

def test_color_only_the_text_not_the_border():
    window = newpad()
    window.should_receive("chgat").with_args(3, 1, 25, curses.A_BOLD | 0).times(1)
    window.should_receive("chgat").with_args(4, 1, 25, curses.A_BOLD | 0).times(1)
    window.should_receive("chgat").with_args(5, 1, 25, curses.A_BOLD | 0).times(1)

    m = Minimap()
    dungeon = fixed_graph()
    m.draw_dungeon(dungeon, window, get_color)

def test_drawing_the_graph_draws_the_current_level_in_a_different_color():
    window = newpad() 
    window.should_receive("chgat").times(3)

    m = Minimap()
    dungeon = fixed_graph()
    m.draw_dungeon(dungeon, window, get_color)

def test_drawing_multiple_branchs():
    pass
