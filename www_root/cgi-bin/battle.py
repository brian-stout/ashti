#!/usr/bin/env python3
import cgi
from mysql_utils import get_connection
from queries import name_lookup
from extra_output import time_string


"""
    print_battles() is a function that generates a table of all the matches that
        occured between the combatants.
"""

def print_battles():
    connection = get_connection()

    #Pulling all the useful information from the fight table.  TIMESTAMPDIFF is used to find
    #   out how much time as elasped between the start and the finish time
    sql = """select combatant_one, combatant_two, winner_id, DATE_FORMAT(start, '%Y-%m-%d %T'),
             TIMESTAMPDIFF(SECOND, start, finish) from fight order by start;"""

    cursor = connection.cursor()
    cursor.execute(sql)

    print("<table style='width:80%'>")
    print("<tr>")
    #Setting up headers for the link to the match, the combatant's names, the victor, and date
    #   information
    print("<th>Match Link</th>")
    print("<th>First Combatant</th>")
    print("<th>Second Combatant</th>")
    print("<th>Victor</th>")
    print("<th>Start Time</th>")
    print("<th>Match Length </th>")
    print("</tr>")

    #battle_link_fmt is used to generate a link which gives all the relevant information
    #   battledetails.py needs to generate a battle detail pages
    battle_link_fmt = """<td style='text-align:center'><a href='battledetails.py?cmbt1={0}&cmbt2={1}&victor={2}&str_time={3}&length={4}'>{5}.</a></td>"""

    #a format string used to generate a link to the combatant's detail page
    name_link_fmt = """<td><a href='combatantid.py?cid={0}'>{1}</a></td>"""

    #A for loop that iterates through the rows and generates the correct table row info
    for index, row in enumerate(cursor.fetchall()):
        #Grabs the names based of the ids
        combatant_1 = name_lookup(row[0])
        combatant_2 = name_lookup(row[1])

        #Quick check to see which combatant won and assign the string to victor to avoid
        #   another lookup
        if row[2] == row[0]:
            victor = combatant_1
        elif row[2] == row[1]:
            victor = combatant_2
        else:
            victor = "Error! Victor data corruption!"

        #Returns a formatted version of how many seconds difference there was between start and ed
        #   of the match time.
        time_elapsed = time_string(row[4])

        print("<tr>")
        print(battle_link_fmt.format(combatant_1,combatant_2,victor,row[3], time_elapsed, index+1))
        print(name_link_fmt.format(row[0], combatant_1))
        print(name_link_fmt.format(row[1], combatant_2))
        print(name_link_fmt.format(row[2], victor))
        print("<td>" + row[3] + "</td>")
        print("<td>" + time_elapsed + "</td>")
        print("</tr>")

    print("</table>")

    cursor.close()
    connection.close()

def main():
    # This is the Head of the HTTP response
    print("Content-type: text/html\n")


    #This begins the Body of hte HTTP Response
    print("<html><head><title>Battles</title>")
    print("<style> table, th, td { border: 0.5px solid black; }</style>")
    print("</head>")
    print("<body>")
    print("<center>")
    print_battles()
    print("<br/><a href='../index.html'>home page</a>")
    print("</center>")
    print("</body></html>")

if __name__ == "__main__":
    main()

