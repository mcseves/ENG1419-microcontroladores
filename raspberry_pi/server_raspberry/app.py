from flask import Flask, request, render_template
from pymongo import MongoClient, DESCENDING
from datetime import datetime

cliente = MongoClient("localhost", 27017)
banco = cliente["monitoramento"]
colecao_rotas = banco["rotas"]
colecao_gps = banco["coordenadas"]


app = Flask(__name__)

if __name__ == '__main__':
    app.run()


@app.route('/')
def inicio():
    cursor = colecao_rotas.find({})
    result = list(cursor)

    results = [{'id': '1', 'situacao': 'normal'}, {'id': '2', 'situacao': 'fora da rota'},
               {'id': '2', 'situacao': 'normal'}]
    return render_template("home.html", results=results)


@app.route("/cadastrar")
def cadastrar_objeto():
    return render_template("calc_route.html")


@app.route("/monitorar")
def monitorar_objeto():
    return render_template("monitorar.html")


@app.route("/salvar_coord/<int:id>", methods=['POST'])
def salvar_coordenadas(id):
    busca = {"id": id}
    resultado = list(colecao_rotas.find(busca))
    if len(resultado) != 0:
        print("Objeto ja existe")
        return render_template('home.html')
    # recebe json da rota via javascript
    rota = request.get_json(force=True)['route']
    documento_objeto = {"id": id, "coordenadas": rota}
    colecao_rotas.insert_one(documento_objeto)

    print("Objeto incluído")
    return render_template('home.html')


@app.route('/enviar_coord', methods=['GET', 'POST'])
def recebe_coordenadas():
    lat, long = request.data.decode("utf-8").split(',')
    coord = {}
    coord["id"] = "1"
    coord["lat"] = lat
    coord["lng"] = long
    coord["data"] = datetime.now()

    colecao_gps.insert_one(coord)
    pega_ultima_coord()

    return render_template('home.html')

def pega_ultima_coord():
    ordenacao = [("data", DESCENDING)]
    return dict(colecao_gps.find_one({}, sort=ordenacao))



