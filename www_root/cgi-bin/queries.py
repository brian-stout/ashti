#!/usr/bin/env python3
from mysql_utils import get_connection


"""
    The get_record() function is responsible for returning a tuple of a combatant's total matches
        fought and total matches won.
"""

def get_record(combatantId):
    connection = get_connection()
    cursor = connection.cursor()
    sql = """ select COUNT(combatant_one) as 'wins' from fight where winner_id = %s"""

    cursor.execute(sql, [combatantId])
    wins = cursor.fetchone()
    wins = wins[0]

    sql = """select COUNT(combatant_one) as 'total' from fight
             where (combatant_one = %s or combatant_two = %s);"""

    variables = (combatantId, combatantId)
    cursor.execute(sql, variables)
    total = cursor.fetchone()
    total = total[0]

    cursor.close()
    connection.close()
    return (wins, total)

"""
    name_lookup() is a function responsible for finding the name of a fighter given a id.
        While it's possible to grab this information via an additional query when the id
        is first queried for, on queries from tables not containing the name it can be clearer
        and more readable to just use a function that grabs the name instead of bloating
        the sql string up with additional selects and joins.
"""

def name_lookup(lookup_id):
    connection = get_connection()
    cursor = connection.cursor()
    sql = """select name from combatant where id = %s"""

    cursor.execute(sql, [lookup_id])
    name = cursor.fetchone()
    name = name[0]

    cursor.close()
    connection.close()
    return name
