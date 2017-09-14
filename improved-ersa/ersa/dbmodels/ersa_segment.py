""" Segment database table """
#   Copyright (c) 2015 by
#   Richard Munoz <rmunoz@nygenome.org>
#   Jie Yuan <jyuan@nygenome.org>
#   Yaniv Erlich <yaniv@nygenome.org>
#
#   All rights reserved
#   GPL license

from sqlalchemy import Column, ForeignKey, Integer, Float
from .base import Base

class Segment(Base):
    """ Table that holds matched segment start and end locations """
    __tablename__ = "ersa_segment"
    id = Column(Integer, primary_key=True)
    result_id = Column(Integer, ForeignKey("ersa_result.id"), index=True)
    chromosome = Column(Integer, nullable=False, index=True)
    bp_start = Column(Integer, nullable=False, index=True)
    bp_end = Column(Integer, nullable=False, index=True)
    length = Column(Float, nullable=False, index=True)
