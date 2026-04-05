import requests

GOAL = "UUUUUUUUURRRRRRRRR" + "FFFFFFFFF" + "DDDDDDDDD" + "LLLLLLLLL" + "BBBBBBBBB"
print(f"State length: {len(GOAL)}")

r = requests.post("http://localhost:8000/solve", json={
    "state": GOAL,
    "algo": "bfs",
    "heuristic": "misplaced"
})
print(r.json())