import json
from urllib import response
from flask import Flask
from flask import jsonify
from flask import request
from flask_cors import CORS, cross_origin
from game import Game

app = Flask(__name__)
game = Game()


@app.route('/')
@cross_origin()
def index():
    obj = {"str": "Hello world"}
    return jsonify(obj), 412


@app.route('/place', methods=["POST"])
@cross_origin()
def place():
    data = request.get_json()
    pos = data['pos']
    try:
        game.place(pos)
    except Exception as e:
        return jsonify({'error': list(e.args)}), 412
    return "OK", 200


@app.route('/end', methods=["POST"])
@cross_origin()
def end():
    game.reset(1)
    return "OK", 200


@app.route('/new', methods=["POST"])
@cross_origin()
def new():
    data = request.get_json()
    side = data["side"]
    if game.reset(side):
        return "OK", 200
    return "Invalid side", 412


@app.route('/board')
@cross_origin()
def get_board():
    board = game.env.board.flatten().tolist()
    response = {
        "board": board,
        "next_player": game.next_player(),
        "over": game.env.gameover,
        "winner": game.winner()
    }
    return jsonify(response)


if __name__ == '__main__':
    app.debug = True
    app.run()
