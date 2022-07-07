from typing import Union, Optional
from fastapi import FastAPI
from sqlalchemy import table
from sqlmodel import Field, Session, SQLModel, create_engine, select, update
from pydantic import BaseModel
from fastapi.responses import JSONResponse

class Toilet(SQLModel, table=True):
    id: Optional[int] = Field(default=None, primary_key=True)
    rotation: float = Field(default=0.0)
    flush: int = Field(default=0)

engine = create_engine("sqlite:///toilet.db")
SQLModel.metadata.create_all(engine)

def is_exist_info(model, id):
    with Session(engine) as session:
        element = session.query(model).filter(model.id == id).first()
        if element is None:
            return False
        return True

app = FastAPI()

@app.post("/tissues/")
def put_tissues(id: int, count: int, angle: int):
    rotation = float(count) + float(angle)/2.0

    if is_exist_info(Toilet, id):
        with Session(engine) as session:
            statement = select(Toilet).where(Toilet.id == id)
            results = session.exec(statement)
            tissue = results.one()

            tissue.rotation = rotation
            session.add(tissue)
            session.commit()
            session.refresh(tissue)

    else:
        tissue = Toilet(id=id, rotation=rotation)
        with Session(engine) as session:
            session.add(tissue)
            session.commit()

    return JSONResponse(status_code=200, content={"status": "ok"})

@app.delete("/tissues/")
def delete_tissues(id: int):
    with Session(engine) as session:
        tissue = session.query(Toilet).filter(Toilet.id == id).first()
        if tissue is None:
            return JSONResponse(status_code=404, content={"status": "not found"})
        session.delete(tissue)
        session.commit()
        return JSONResponse(status_code=200, content={"status": "ok"})

@app.post("/toilets/")
def put_toilets(id: int, blocked: bool):
    if is_exist_info(Toilet, id):
        with Session(engine) as session:
            statement = select(Toilet).where(Toilet.id == id)
            results = session.exec(statement)
            toilet = results.one()

            if blocked == True:
                toilet.flush += 1

            else:
                toilet.flush = 0

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

@app.get("/toilets/")
def get_toilets(id: int):
    with Session(engine) as session:
        toilet = session.query(Toilet).filter(Toilet.id == id).first()
        if toilet is None:
            return JSONResponse(status_code=404, content={"status": "not found"})
        return JSONResponse(status_code=200, content={"status": "ok", "rotation":toilet.rotation, "flush": toilet.flush})

@app.get("/toilets/list/")
def get_toilets_list():
    with Session(engine) as session:
        statement = select(Toilet)
        results = session.exec(statement)
        toilets = results.all()
        toilets = [toilet.id for toilet in toilets]
        return JSONResponse(status_code=200, content={"status": "ok", "toilets": toilets})