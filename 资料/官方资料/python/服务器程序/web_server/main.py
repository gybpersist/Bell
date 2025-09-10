from typing import Union

from fastapi import FastAPI, WebSocket
from fastapi.staticfiles import StaticFiles

app = FastAPI()

app.mount("/static", StaticFiles(directory="static"), name="static")


@app.get("/")
def read_root():
    return {"Hello": "World"}


@app.get("/items/{item_id}")
def read_item(item_id: int, q: Union[str, None] = None):
    return {"item_id": item_id, "q": q}


@app.websocket("/ws/from_esp")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    print("connection from esp32")
    global from_esp_ws_conn
    from_esp_ws_conn = websocket
    while True:
        data = await websocket.receive_bytes()
        if to_browser_ws_conn != None:
            try:
                await to_browser_ws_conn.send_bytes(data)
            except Exception as e:
                print("error") 

@app.websocket("/ws/to_browser")
async def websocket_to_browser(websocket: WebSocket):
    await websocket.accept()
    print("connection from to_browser")
    global to_browser_ws_conn
    to_browser_ws_conn = websocket
    while True:
        data = await websocket.receive_bytes()
        if from_esp_ws_conn != None:
            try:
                await from_esp_ws_conn.send_bytes(data)
            except Exception as e:
                print("error") 

@app.websocket("/ws/image")
async def websocket_camera(websocket: WebSocket):
    await websocket.accept()
    print("conn from image")
    while True:
        data = await websocket.receive_bytes()
        file_path = "./static/image.jpeg"  
        try:
            with open(file_path, "wb") as f:
                f.write(data)
        except Exception as e:
            print("1 error")    
