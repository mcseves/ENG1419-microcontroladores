from flask import Flask, flash, request, render_template, session, g, redirect, url_for
from pymongo import MongoClient, DESCENDING
from datetime import datetime
from twilio.rest import Client
from functools import wraps

cliente = MongoClient("localhost", 27017)
banco = cliente["monitoramento"]
colecao_rotas = banco["rotas"]
colecao_gps = banco["coordenadas"]
controle = 0

app = Flask(__name__)
app.secret_key = b'jajasaj82qwi9021jsa##$#@'

if __name__ == '__main__':
    app.run()


@app.route('/')
def home():
    if not session.get('logged_in'):
        return render_template('login.html')
    else:
        return inicio()


@app.route('/login', methods=['POST'])
def do_admin_login():
    if request.form['password'] == 'password' and request.form['username'] == 'admin':
        session['logged_in'] = True
        return inicio()
    else:
        flash('wrong password!')
    return home()

# Funcao que é executada de tempos em tempos, verificando se a ultima coordenada do GPS
# gravada no banco está ou não dentro da rota definida
@app.route('/inicio')
# @login_required
def inicio():
    # Pega rota do objeto no banco de dados
    cursor_rotas = colecao_rotas.find({})
    lista_rotas = list(cursor_rotas)

    # Pega última coordenada vinda do GPS gravada no banco
    coord = pega_ultima_coord_gps()

    # verifica a rota, retornando uma lista com dicionarios para popular a tabela da pagina inicial
    tabela = verificar_rota(lista_rotas, coord['lat'], coord['lng'])

    return render_template("home.html", results=tabela)


@app.route("/cadastrar")
# @login_required
def cadastrar_objeto():
    return render_template("calc_route.html")


@app.route("/monitorar")
# @login_required
def monitorar_objeto():
    coord = pega_ultima_coord_gps()
    local = consultar_rota()
    rota = local[0]["coordenadas"]

    return render_template("monitorar.html", coord=coord, rota=rota)


# Salva as coordenadas da rota definida no banco de dados
# Formato salvo: dict com as chaves 'id', 'coordenadas' (lista de dicts com 'lat' e 'lng')
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


# Recebe coordenadas do GPS via requisição e grava no banco de dados
@app.route('/enviar_coord', methods=['GET', 'POST'])
def recebe_coordenadas():
    lat, long = request.data.decode("utf-8").split(',')
    coord = {'id': '1', 'lat': lat, 'lng': long, 'data': datetime.now()}

    colecao_gps.insert_one(coord)

    return render_template('home.html')


# Pega a ultima coordenada do GPS pela data de inclusao
def pega_ultima_coord_gps():
    ordenacao = [("data", DESCENDING)]
    return dict(colecao_gps.find_one({}, sort=ordenacao))


# Verifica se a coordenada do GPS está dentro da rota,
# envia um SMS caso não e monta um dicionario com essa informacao
# retorno: dict com as keys 'id' e 'situacao' ('normal' ou 'fora da rota')
def verificar_rota(rota, latitude_gps, longitude_gps):
    global controle
    resultado = {'id': '1'}
    distancias = []
    dist_pontos_adjacentes = []
    latitudes_rota = []
    longitudes_rota = []
    coord_rota = rota[0]["coordenadas"]
    for coord in coord_rota:
        latitudes_rota.append(float(coord['lat']))
        longitudes_rota.append(float(coord['lng']))

    for i in range(len(latitudes_rota) - 1):
        dist_pontos_adjacentes.append(
            ((latitudes_rota[i] - latitudes_rota[i + 1]) ** 2 + (longitudes_rota[i] - longitudes_rota[i + 1]) ** 2) ** (
                    1 / 2))
    for i in range(len(latitudes_rota)):
        distancias.append(
            ((latitudes_rota[i] - float(latitude_gps)) ** 2 + (longitudes_rota[i] - float(longitude_gps)) ** 2) ** (1 / 2))

    distancias.sort()  # Ordenando as distancias entre o ponto GPS e os pontos da rota
    dist_pontos_adjacentes.sort()  # Ordenando as distancias entre os pontos adjacentes da rota

    # Comparação da menor distancia encontrada entre a localização
    # do GPS e a maior distância entre dois pontos adjacantes da rota.
    if dist_pontos_adjacentes[-1] < distancias[0]:
        controle += 1
        if controle < 5:
            enviar_sms('Você está fora da rota definida. Volte imediatamente para a rota!')
            resultado['situacao'] = 'fora da rota'

        else:
            enviar_sms('DESTRUIR')
            print('Autodestruindo!!!!  - Enviado pelo servidor')
            resultado['situacao'] = 'fora da rota'

    if 'situacao' not in resultado:
        resultado['situacao'] = 'normal'

    return resultado

@app.route("/autodestruir")
# @login_required
def acao_autodestruir():
    print('Autodestruindo!!!!  - Enviado pelo site')
    enviar_sms('DESTRUIR')

    results = {'id': '1', 'situacao': 'Destruindo...'}
    return render_template("home.html", results=results)


# Envia SMS
def enviar_sms(texto):
    twilio_number = '+12016544082'
    to = '+5521988526176'
    # Your Account SID from twilio.com/console
    account_sid = "ACc1e15d9f1e5c6e4ef7646670b3bd9f40"
    # Your Auth Token from twilio.com/console
    auth_token = "8007cd98003bfa8d7ecacd8dc5911a4a"

    client = Client(account_sid, auth_token)

    message = client.messages.create(
        to=to,
        from_=twilio_number,
        body=texto)

    print(message.sid)


@app.route('/destruiu', methods=['GET', 'POST'])
def aviso_destruicao():
    dado = request.data.decode("utf-8")
    if dado == '1':
        results = [{'id': '1', 'situacao': 'Destruído'}]
        return render_template("home.html", results=results)
    return render_template("home.html")


def consultar_rota():
    # Pega rota do objeto no banco de dados
    cursor_rotas = colecao_rotas.find({})
    return list(cursor_rotas)
