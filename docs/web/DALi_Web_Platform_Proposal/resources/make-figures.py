#!/usr/bin/env python3
"""Generate editable, original SVG illustrations for the Korean proposal."""
from pathlib import Path
from html import escape

OUT = Path(__file__).resolve().parent
INK, MUTED, BG, LINE = '#15253f', '#5d6c82', '#f4f7fb', '#dbe3ef'
TEAL, BLUE, PURPLE, AMBER = '#087e8b', '#2563eb', '#7553c5', '#ad6908'


class Figure:
    def __init__(self, height, title, subtitle):
        self.height = height
        self.parts = [f'<svg xmlns="http://www.w3.org/2000/svg" width="1600" height="{height}" viewBox="0 0 1600 {height}">',
            '<defs><marker id="arrow" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="8" markerHeight="8" orient="auto-start-reverse"><path d="M0 0 10 5 0 10z" fill="context-stroke"/></marker>',
            '<linearGradient id="hero" x1="0" y1="1" x2="1" y2="0"><stop stop-color="#303c96"/><stop offset=".55" stop-color="#6854ce"/><stop offset="1" stop-color="#bd7cdd"/></linearGradient>',
            '<pattern id="grid" width="24" height="24" patternUnits="userSpaceOnUse"><circle cx="1" cy="1" r="1" fill="#34425b"/></pattern></defs>',
            '<style>text{font-family:"Noto Sans CJK KR","Noto Sans CJK JP",sans-serif}</style>']
        self.rect(0, 0, 1600, height, BG, rx=0)
        self.text(title, 60, 66, 34, weight=700)
        self.text(subtitle, 60, 108, 21, MUTED)

    def add(self, s):
        self.parts.append(s)

    def rect(self, x, y, w, h, fill='white', stroke=None, rx=16):
        border = f' stroke="{stroke}" stroke-width="2"' if stroke else ''
        self.add(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{rx}" fill="{fill}"{border}/>')

    def text(self, s, x, y, size=24, color=INK, weight=400, anchor='start'):
        self.add(f'<text x="{x}" y="{y}" font-size="{size}" fill="{color}" font-weight="{weight}" text-anchor="{anchor}">{escape(str(s))}</text>')

    def lines(self, values, x, y, size=23, color=MUTED, step=35, weight=400):
        for i, value in enumerate(values):
            self.text(value, x, y+i*step, size, color, weight)

    def arrow(self, x1, y1, x2, y2, color=TEAL, dashed=False):
        dash = ' stroke-dasharray="9 7"' if dashed else ''
        self.add(f'<path d="M{x1} {y1} L{x2} {y2}" fill="none" stroke="{color}" stroke-width="3"{dash} marker-end="url(#arrow)"/>')

    def path(self, d, color=LINE, width=2, dashed=False):
        dash = ' stroke-dasharray="8 6"' if dashed else ''
        self.add(f'<path d="{d}" fill="none" stroke="{color}" stroke-width="{width}"{dash}/>')

    def chip(self, label, x, y, w, color=TEAL, fill='#e6f4f4'):
        self.rect(x, y, w, 38, fill, rx=19)
        self.text(label, x+w/2, y+26, 19, color, 600, 'middle')

    def save(self, name):
        (OUT / f'{name}.svg').write_text('\n'.join(self.parts)+ '\n</svg>\n', encoding='utf-8')


f = Figure(700, '디자인 전달부터 실행 확인까지, 하나의 작업 흐름으로', '필요성 설명을 위한 업무 흐름 예시 · 제안하는 운영 방식')
f.rect(60, 146, 1480, 208, 'white', LINE)
f.chip('분리된 작업 흐름', 84, 166, 205, '#775b48', '#f4ede7')
old = [('디자인 전달', '시안·명세를 공유'), ('화면 구현', '코드로 배치 재현'), ('환경 준비·빌드', 'SDK·의존성 설정'), ('실행 결과 확인', '기기·플랫폼 확인'), ('수정 요청', '차이를 다시 전달')]
for i, (title, desc) in enumerate(old):
    x = 88 + i*290
    f.rect(x, 230, 258, 96, '#f7f8fa', LINE, 12)
    f.text(title, x+129, 267, 26, INK, 600, 'middle')
    f.text(desc, x+129, 301, 20, MUTED, anchor='middle')
    if i < 4: f.arrow(x+262, 279, x+285, 279, '#a2aebf')
f.rect(60, 390, 1480, 248, '#e9f5f6', '#b9dfe2')
f.chip('구축 후 목표 흐름', 84, 410, 225)
new = [('브라우저에서 편집', 'Scene·코드를 같은 곳에서 수정'), ('DALi 프리뷰 확인', '배치·상태·동작을 빠르게 확인'), ('프로젝트 저장', 'Scene·리소스·로직을 관리'), ('공유·기기 검증', '버전을 공유하고 Tizen에 적용')]
for i, (title, desc) in enumerate(new):
    x = 88 + i*364
    f.rect(x, 480, 330, 110, 'white', '#a6d3d7', 12)
    f.text(title, x+165, 522, 26, TEAL, 700, 'middle')
    f.text(desc, x+165, 559, 20, MUTED, anchor='middle')
    if i < 3: f.arrow(x+334, 534, x+359, 534)
f.text('작은 변경을 짧게 반복하고, 만들어진 Scene을 실제 앱에서 재사용하는 것을 목표로 한다.', 800, 674, 23, TEAL, 600, 'middle')
f.save('01-workflow')

f = Figure(950, '제안 아키텍처: 서버는 빌드·저장, 브라우저는 실행·렌더링', '목표 구성 · WebGL / WebGPU 선택과 무관하게 공통으로 사용할 플랫폼')
f.rect(60, 150, 650, 625, '#edf4ff', '#bfd3f4')
f.rect(830, 150, 710, 625, '#edf7f3', '#c0dfd2')
f.text('사용자 PC · 지원 브라우저', 88, 196, 28, BLUE, 700)
f.text('Ubuntu 기반 서비스', 858, 196, 28, TEAL, 700)
f.rect(90, 226, 590, 175, 'white', '#bfd3f4')
f.text('웹 에디터', 118, 270, 29, INK, 700)
f.lines(['코드 · Scene tree · 속성 · 이벤트 편집', '파일 업로드 · 저장 · 빌드 요청 · 리뷰'], 118, 315, 24)
f.arrow(385, 415, 385, 466, BLUE)
f.text('편집 명령을 런타임에 반영', 410, 448, 20, BLUE)
f.rect(90, 482, 590, 252, 'white', '#bfd3f4')
f.text('DALi WASM 런타임', 118, 527, 29, INK, 700)
f.rect(118, 550, 532, 54, '#edf4ff', rx=10)
f.text('dali-core · dali-adaptor · dali-ui', 384, 586, 24, BLUE, 600, 'middle')
f.rect(118, 619, 532, 54, '#edf4ff', rx=10)
f.text('WebGL 2 / WebGPU → 사용자 PC의 GPU', 384, 655, 23, BLUE, 600, 'middle')
f.text('사용자 PC에 SDK·컴파일러 설치 없이 이용하는 것을 목표로 함', 118, 712, 19, MUTED)
for y, title, detail in [(226, 'HTTPS 서비스 · API', ['인증·권한 · 프로젝트·워크스페이스', '파일·Scene API · WebSocket·터미널']),
                         (406, '격리된 빌드 실행 환경', ['DALi·Emscripten 버전 고정 · 빌드 큐', '캐시 재사용 · 로그·진단 · 자원 제한']),
                         (586, '프로젝트 · 결과물 저장소', ['Git·커밋 · Scene·리소스·사용자 로직', '버전별 WASM·JS·리소스 · 공유 링크'])]:
    f.rect(860, y, 650, 152, 'white', '#c0dfd2')
    f.text(title, 887, y+39, 27, INK, 700)
    f.lines(detail, 887, y+81, 23, step=34)
f.arrow(1185, 383, 1185, 401)
f.arrow(1185, 563, 1185, 581)
f.arrow(690, 302, 820, 302, BLUE)
f.arrow(820, 342, 690, 342, BLUE)
f.chip('①', 727, 255, 54, BLUE, '#dceafe')
f.arrow(820, 645, 690, 645)
f.chip('②', 727, 597, 54)
f.lines(['① HTTPS / WSS: 저장·빌드 요청, 상태·로그 수신', '② 버전이 고정된 실행 파일과 리소스 다운로드'], 88, 829, 24, INK, 39)
f.text('Tizen 앱 적용은 별도 네이티브 빌드·기기 검증 단계와 연결한다.', 88, 920, 23, MUTED)
f.save('04-architecture')

f = Figure(930, '그래픽 API 선택은 공통 플랫폼과 분리해 검증', '기술 추진 방향 · WebGPU는 Vulkan 바이너리를 그대로 실행하는 인터페이스가 아님')
f.rect(320, 150, 960, 130, 'white', LINE)
f.text('DALi Scene · 레이아웃 · 애니메이션', 800, 202, 31, INK, 700, 'middle')
f.text('공통 Graphics API와 자원 모델을 확장 지점으로 검토', 800, 246, 25, MUTED, anchor='middle')
f.path('M800 280 V315 H280 V351 M800 315 V351 M800 315 H1320 V351', '#a5b4ca', 3)
columns = [
    (60, BLUE, '#edf4ff', 'WebGL 2 경로', '초기 지원 범위 확보', ['GLES 호출의 Web 대응', 'GLSL ES 3.00 기반 셰이더', 'WebGL 2 · ES 3.0에 가까운 범위']),
    (580, PURPLE, '#f2effb', 'WebGPU 경로', '별도 backend 타당성 검증', ['자원·파이프라인·명령 매핑', 'WGSL 작성 / 변환 검증', 'WebGPU 기능·한계 협상']),
    (1100, TEAL, '#eaf6f3', '네이티브 경로', '최종 적용·검증 경로 유지', ['기존 GLES / Vulkan backend', '네이티브 셰이더·GPU 드라이버', 'Tizen 기기의 기능·성능 확인'])]
for x, color, fill, title, status, rows in columns:
    f.rect(x, 352, 440, 358, 'white', LINE)
    f.rect(x, 352, 440, 107, fill, rx=16)
    f.text(title, x+220, 398, 29, color, 700, 'middle')
    f.text(status, x+220, 435, 21, color, anchor='middle')
    for i, row in enumerate(rows):
        f.rect(x+24, 481+i*68, 392, 52, fill, rx=10)
        f.text(row, x+220, 515+i*68, 21, color, anchor='middle')
f.arrow(1320, 750, 800, 750, PURPLE, True)
f.text('Vulkan 설계·셰이더 자산의 재사용 가능성 검토', 1060, 790, 22, PURPLE, anchor='middle')
f.rect(60, 824, 1480, 68, '#fff5df', '#edd9b4', 12)
f.text('채택 기준: 필요한 기능의 지원 여부 + 셰이더 변환 가능성 + 실제 브라우저 측정 결과', 800, 867, 26, AMBER, 600, 'middle')
f.save('05-backend-strategy')

f = Figure(1060, '구축 후 활용 예시 · Scene 기반 시각 저작', '기능과 작업 흐름을 설명하기 위해 제작한 컨셉 시안')
f.chip('컨셉 시안 · 미구현', 1286, 75, 254, AMBER, '#fff0d3')
f.rect(30, 145, 1540, 850, '#101829', '#29374d', 18)
f.rect(30, 145, 1540, 64, '#182236', rx=16)
f.text('DALi Studio', 56, 187, 28, '#8de0da', 700)
f.text('생활 서비스 / Home.scene', 295, 185, 22, '#d6dfed')
for x, label, w, fill, color in [(1080, '저장', 88, '#25344c', '#e3eaf5'), (1180, '실행', 88, '#25344c', '#e3eaf5'),
                                  (1280, '공유', 88, '#25344c', '#e3eaf5'), (1380, '코드 내보내기', 168, '#6dd7c1', '#102a29')]:
    f.rect(x, 159, w, 38, fill, rx=7); f.text(label, x+w/2, 185, 18, color, 600, 'middle')
f.rect(30, 210, 270, 738, '#141e30', rx=0)
f.rect(300, 210, 900, 738, '#101929', rx=0)
f.rect(300, 263, 900, 541, 'url(#grid)', rx=0)
f.rect(1200, 210, 370, 738, '#141e30', rx=0)
f.text('SCENE TREE', 54, 250, 18, '#8da0bb', 600)
tree = [('⌄ Home', 0), ('Header', 1), ('HeroCard', 1), ('Title', 2), ('Subtitle', 2), ('ActionButton', 2), ('QuickLinks', 1)]
for i, (name, indent) in enumerate(tree):
    y = 300+i*43
    if name == 'HeroCard': f.rect(42, y-28, 244, 38, '#234153', '#59cfc4', 6)
    f.text(name, 57+indent*22, y, 21, '#ecf2fa' if name == 'HeroCard' else '#b3c0d4')
f.path('M51 625 H277', '#2c3c54')
f.text('컴포넌트 추가', 54, 666, 20, '#dbe5f4', 600)
for i, label in enumerate(['뷰', '텍스트', '이미지', '버튼']):
    x, y = 51+(i%2)*118, 690+(i//2)*74
    f.rect(x, y, 108, 59, '#213047', '#34475f', 8)
    f.text(label, x+54, y+38, 21, '#d6e4f5', anchor='middle')
f.text('캔버스', 324, 250, 20, '#dbe5f4', 600)
f.text('1280 × 720   |   맞춤 보기', 1174, 250, 19, '#8da0bb', anchor='end')
f.rect(372, 300, 756, 448, '#f4f6fd', rx=12)
f.text('DALi Home', 404, 342, 23, '#1c2f52', 700)
f.text('홈     컬렉션     설정', 1094, 341, 17, '#6a7b98', anchor='end')
f.rect(408, 372, 684, 242, 'url(#hero)', rx=22)
f.add('<defs><clipPath id="heroClip"><rect x="408" y="372" width="684" height="242" rx="22"/></clipPath></defs><g clip-path="url(#heroClip)"><circle cx="1000" cy="423" r="105" fill="#c4a4e8" opacity=".18"/><circle cx="1047" cy="580" r="124" fill="#7ce8df" opacity=".2"/></g>')
f.chip('나를 위한 추천', 437, 393, 156, '#e9e5ff', '#ffffff22')
f.text('오늘의 추천을', 437, 474, 39, 'white', 700)
f.text('만나보세요', 437, 526, 39, 'white', 700)
f.rect(439, 550, 152, 40, 'white', rx=20)
f.text('시작하기  →', 515, 578, 19, '#52439a', 600, 'middle')
f.rect(402, 366, 696, 254, 'none', '#4ee1d0', 25)
for x in [402, 750, 1098]:
    for y in [366, 620]: f.rect(x-5, y-5, 10, 10, '#dbfff8', '#258b85', 1)
for x in [402, 1098]: f.rect(x-5, 493-5, 10, 10, '#dbfff8', '#258b85', 1)
for i, (label, color) in enumerate([('이어보기', '#dae8ff'), ('즐겨찾기', '#e5defc'), ('새로운 소식', '#d5eee7')]):
    x=409+i*235
    f.rect(x, 641, 215, 74, color, rx=12)
    f.rect(x+16, 659, 37, 37, '#ffffff99', rx=10)
    f.text(label, x+66, 686, 20, '#334969', 600)
f.text('스냅 8 px  ·  드래그로 위치 변경  ·  속성 변경 즉시 프리뷰', 750, 783, 19, '#aab9ce', anchor='middle')
f.rect(300, 805, 900, 143, '#172235', rx=0)
f.text('상태 · 애니메이션', 324, 845, 20, '#dbe5f4', 600)
for i, label in enumerate(['기본', '포커스', '눌림']):
    x=336+i*275
    f.rect(x, 865, 235, 46, '#263550', rx=9)
    f.text(label, x+117, 896, 21, '#d6e4f5', anchor='middle')
    if i < 2: f.arrow(x+244, 888, x+267, 888, '#6dd7c1')
f.text('PROPERTIES', 1225, 250, 18, '#8da0bb', 600)
f.text('HeroCard', 1225, 295, 26, '#eef5ff', 700)
f.text('배치', 1225, 340, 20, '#dbe5f4', 600)
for i, (key, value) in enumerate([('X / Y', '80 / 160'), ('너비 / 높이', '1120 / 400'), ('모서리 반경', '24'), ('배경', '선형 그라데이션')]):
    y=361+i*63
    f.text(key, 1227, y+22, 18, '#92a5bf')
    f.rect(1350, y, 193, 40, '#23314a', '#384961', 6)
    f.text(value, 1362, y+27, 18, '#e7effa')
f.path('M1226 625 H1544', '#2c3c54')
f.text('이벤트', 1225, 666, 20, '#dbe5f4', 600)
f.text('onClick', 1228, 708, 19, '#7cddd1')
f.rect(1226, 728, 317, 52, '#23314a', '#384961', 8)
f.text('Navigate → Detail', 1242, 762, 22, '#e7effa')
f.lines(['이벤트는 선언으로 저장', '앱 로직은 별도 코드에서 연결'], 1227, 821, 19, '#92a5bf', 32)
f.rect(30, 949, 1540, 46, '#183d40', rx=0)
f.text('Home.scene  ·  12개 노드  ·  공통 Scene 모델', 54, 979, 18, '#a5e6d8')
f.text('사용자 로직과 생성 코드 분리', 1540, 979, 18, '#a5e6d8', anchor='end')
f.text('시각 편집 → Scene 저장 → 앱 로직 연결 → DALi 앱에 재사용하는 흐름을 목표로 한다.', 800, 1036, 23, TEAL, 600, 'middle')
f.save('03-authoring-concept')

print('Generated 4 original SVG figures.')
