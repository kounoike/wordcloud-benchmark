import sys
import timeit
from cloudia import Cloudia

id_ = sys.argv[1] if len(sys.argv) > 1 else 148
num = 3

setup = f"""
gc.enable()
import io
from cloudia import Cloudia
import matplotlib.pyplot as plt
from fugashi import Tagger
from typing import Any, List, Tuple
import re
NUM_REGEX = re.compile('^[0-9]+$')

def parser(text: str, single_words: List[str], extract_postags: List[str], stop_words: List[str], parser) -> List[str]:
    parser = Tagger()
    for x in ['"', ';', ',', '(', ')', '\u3000']:
        text = text.replace(x, ' ')
    text = text.lower()
    return [x for x in map(str, parser.parseToNodeList(text)) if len(x) > 1 and not NUM_REGEX.match(x) and x not in stop_words]


class MyCloudia(Cloudia):
    def get_png_buf(self, dark_theme: bool = False, title_size: int = 12, row_num: int = 3, figsize_rate: int = 2):
        buf = io.BytesIO()
        wc = self.make_wordcloud(dark_theme, figsize_rate)
        self.make_fig(wc, dark_theme, title_size, row_num, figsize_rate)
        plt.savefig(buf, facecolor=self._color(dark_theme), pad_inches=0.0, bbox_inches="tight")
        return buf

def benchmark_parseonly(text):
    c = MyCloudia(text)

def benchmark_drawonly(cloudia_obj):
    cloudia_obj.get_png_buf()

with open("data/{id_}.txt", encoding="utf-8") as f:
    text = f.read()
# c = MyCloudia(text)
c = MyCloudia(text, parse_func=parser)
"""

with open(f"data/{id_}.txt", encoding="utf-8") as f:
    text = f.read()

print(f"id_: {id_}")
print(f"text length: {len(text)}")
# c = Cloudia(text)
# print(f"words: {len(c.wd.counter_list[0])}")

from fugashi import Tagger
from typing import Any, List, Tuple
import re
NUM_REGEX = re.compile('^[0-9]+$')

def parser(text: str, single_words: List[str], extract_postags: List[str], stop_words: List[str], parser) -> List[str]:
    parser = Tagger()
    for x in ['"', ';', ',', '(', ')', '\u3000']:
        text = text.replace(x, ' ')
    text = text.lower()
    return [x for x in map(str, parser.parseToNodeList(text)) if len(x) > 1 and not NUM_REGEX.match(x) and x not in stop_words]

fc = Cloudia(text, parse_func=parser)
print(f"words by Fugashi: {len(fc.wd.counter_list[0])}")

print(timeit.timeit('benchmark_parseonly(text)', number=num, setup=setup) / num)
print(timeit.timeit('benchmark_drawonly(c)', number=num, setup=setup) / num)
