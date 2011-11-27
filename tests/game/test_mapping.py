from flexmock import flexmock

from noobhack.game.mapping import Map, Level

from tests.utils import level_chain

def level(dlvl):
    return flexmock(Level(dlvl))

def test_walking_down_a_single_level_in_the_same_branch_that_doesnt_exists():
    m = Map(level(1), 1, 1)

    second = flexmock(Level(2))
    flexmock(Level).new_instances(second).once

    m.current.should_receive("add_stairs").with_args(second, (1, 1)).once
    second.should_receive("add_stairs").with_args(m.current, (2, 2)).once

    m.travel_by_stairs(2, (2, 2))

    assert m.current == second

def test_moving_changes_current_position():
    m = Map(level(1), 1, 1)
    m.move(5, 5)
    
    assert m.location == (5, 5)

def test_walking_up_a_single_level_in_the_same_branch_that_already_exists():
    first = level(1)
    m = Map(first, 1, 1)

    second = flexmock(Level(2))
    second.should_receive("add_stairs").once
    first.should_receive("add_stairs").once
    flexmock(Level).new_instances(second).once

    # Travel down once to create the second level
    m.travel_by_stairs(2, (2, 2))

    second.should_receive("has_stairs_at").and_return(True).once
    second.should_receive("stairs_at").and_return(first).once

    # Now traveling back up should go back to the first
    m.travel_by_stairs(1, (1, 1))

    assert len(m.levels) == 2
    assert m.current == first

def test_traveling_down_an_existing_link_doesnt_create_a_new_link_or_level():
    m = Map(level(1), 1, 1)

    second = Level(2)

    first = flexmock(Level(1))
    first.should_receive("has_stairs_at").and_return(True).once
    first.should_receive("stairs_at").and_return(second)
    first.should_receive("add_stairs").never

    m.current = first

    m.travel_by_stairs(2, (2, 2))
    
    assert m.current == second

def test_traveling_down_when_a_link_doesnt_exist_creates_a_new_link_and_level():
    m = Map(level(1), 1, 1)

    first = flexmock(Level(1))
    first.should_receive("has_stairs_at").and_return(False).once
    first.should_receive("add_stairs").with_args(object, (1, 1))

    m.current = first

    flexmock(Level).new_instances(flexmock(Level(5))).once

    m.travel_by_stairs(2, (2, 2))

    assert len(m.levels) == 2

def test_teleporting_to_an_undiscovered_level_doesnt_link():
    m = Map(level(1), 1, 1)

    first = flexmock(Level(1))
    m.current = first

    first.should_receive("add_stairs").never
    flexmock(Level).new_instances(flexmock(Level(5))).once

    m.travel_by_teleport(5, (5, 5))

    assert m.current.dlvl == 5
    assert len(m.levels) == 2 

def test_moving_up_stairs_to_an_unknown_level_creates_a_new_link_and_level():
    m = Map(level(1), 5, 5)

    fifth = flexmock(Level(5))
    fifth.should_receive("has_stairs_at").and_return(False).once
    fifth.should_receive("add_stairs").with_args(object, (5, 5))

    m.current = fifth

    m.travel_by_stairs(4, (4, 4))

    assert m.current.dlvl == 4
    assert len(m.levels) == 2

def test_moving_upstairs_from_to_a_known_level_that_isnt_yet_linked_with_the_current():
    m = Map(level(1), 2, 2)

    first = flexmock(Level(1))
    second = flexmock(Level(2))

    m.levels = set([first, second])
    m.current = second 

    first.should_receive("add_stairs").with_args(second, (1, 1)).once
    second.should_receive("add_stairs").with_args(first, (2, 2)).once

    m.travel_by_stairs(1, (1, 1))

    assert m.current.dlvl == 1

def test_switching_a_branch_changes_the_current_level_to_a_new_branch():
    m = Map(level(1), 2, 2)

    second = flexmock(Level(2))
    second.should_receive("change_branch_to").with_args("mines").once
    m.levels.add(second)
    m.current = second
    m.change_branch_to("mines")

def test_moving_down_main_then_up_one_and_down_to_the_mines_before_mines_are_identified():
    levels = level_chain(3, "main")
    m = Map(levels[-1], 3, 2)
    m.levels = set(levels)
    m.travel_by_stairs(2, (2, 3))

    assert len(m.levels) == 3

    m.move(4, 4)
    m.travel_by_stairs(3, (4, 4))

    assert len(m.levels) == 4

    m.change_branch_to("mines")

    assert len([l for l in m.levels if l.branch == "main"]) == 3
    assert len([l for l in m.levels if l.branch == "mines"]) == 1
