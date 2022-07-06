from typing import Union, Optional
from fastapi import FastAPI
from sqlalchemy import table
from sqlmodel import Field, Session, SQLModel, create_engine, select, update
from pydantic import BaseModel
from fastapi.responses import JSONResponse

class Tissue(SQLModel, table=True):
    id: Optional[int] = Field(default=None, primary_key=True)
    rotation: float

class Toilet(SQLModel, table=True):
    id: Optional[int] = Field(default=None, primary_key=True)
    blocked: bool

engine = create_engine("sqlite:///toilet.db")
SQLModel.metadata.create_all(engine)

def is_exist_info(model, id):
    with Session(engine) as session:
        element = session.query(model).filter(model.id == id).first()
        if element is None:
            return False
        return True

app = FastAPI()

@app.get("/tissues/")
def get_tissues(id: int):
    with Session(engine) as session:
        tissue = session.query(Tissue).filter(Tissue.id == id).first()
        if tissue is None:
            return JSONResponse(status_code=404, content={"status": "not found"})
        return JSONResponse(status_code=200, content={"status": "ok", "rotation": tissue.rotation})

@app.post("/tissues/")
def put_tissues(id: int, count: int, angle: int):
    rotation = float(count) + float(angle)/2.0

    if is_exist_info(Tissue, id):
        with Session(engine) as session:
            statement = select(Tissue).where(Tissue.id == id)
            results = session.exec(statement)
            tissue = results.one()

            tissue.rotation = rotation
            session.add(tissue)
            session.commit()
            session.refresh(tissue)

    else:
        tissue = Tissue(id=id, rotation=rotation)
        with Session(engine) as session:
            session.add(tissue)
            session.commit()

    return JSONResponse(status_code=200, content={"status": "ok"})

@app.delete("/tissues/")
def delete_tissues(id: int):
    with Session(engine) as session:
        tissue = session.query(Tissue).filter(Tissue.id == id).first()
        if tissue is None:
            return JSONResponse(status_code=404, content={"status": "not found"})
        session.delete(tissue)
        session.commit()
        return JSONResponse(status_code=200, content={"status": "ok"})

@app.get("/toilets/")
def get_toilets(id: int):
    with Session(engine) as session:
        toilet = session.query(Toilet).filter(Toilet.id == id).first()
        if toilet is None:
            return JSONResponse(status_code=404, content={"status": "not found"})
        return JSONResponse(status_code=200, content={"status": "ok", "blocked": toilet.blocked})

@app.post("/toilets/")
def put_toilets(id: int, blocked: bool):
    if is_exist_info(Toilet, id):
        with Session(engine) as session:
            statement = select(Toilet).where(Toilet.id == id)
            results = session.exec(statement)
            toilet = results.one()

            toilet.blocked = blocked
            session.add(toilet)
            session.commit()
            session.refresh(toilet)

    else:
        toilet = Toilet(id=id, blocked=blocked)
        with Session(engine) as session:
            session.add(toilet)
            session.commit()

    return JSONResponse(status_code=200, content={"status": "ok"})

@app.delete("/toilets/")
def delete_toilets(id: int):
    with Session(engine) as session:
        toilet = session.query(Toilet).filter(Toilet.id == id).first()
        if toilet is None:
            return JSONResponse(status_code=404, content={"status": "not found"})
        session.delete(toilet)
        session.commit()
        return JSONResponse(status_code=200, content={"status": "ok"})