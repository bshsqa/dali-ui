#!/usr/bin/env python3
#
# Copyright (c) 2026 Samsung Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

"""Check generated IDs, cast maps, installed headers and the native app's log."""
import argparse
import json
from pathlib import Path
import re
import subprocess
import uuid

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--core", required=True, type=Path, help="core query-interfaces/dali3-core directory")
parser.add_argument("--ui", required=True, type=Path, help="UI query-interfaces/dali3-ui-foundation directory")
parser.add_argument("--app", required=True, type=Path, help="sample query-interfaces/query-interface.example directory")
parser.add_argument("--runtime-log", required=True, type=Path)
parser.add_argument("--sdk", required=True, type=Path)
args = parser.parse_args()

expected = {
    "Dali::IPlayback": (args.core, ["Play", "Pause", "Stop"], "dali/public-api/interfaces/playback.generated.h"),
    "Dali::Ui::ITextContent": (args.ui, ["SetText", "GetText"], "dali-ui-foundation/public-api/interfaces/text-content.generated.h"),
    "QueryInterfaceSample::IResettable": (args.app, ["ResetContent"], "interfaces/resettable.generated.h"),
}
entries = {}
log = args.runtime_log.read_text()
for name, (directory, methods, header) in expected.items():
    manifest = json.loads((directory / "interfaces.json").read_text())
    entry = manifest["interfaces"][name]
    entries[name] = entry
    assert [m["name"] for m in entry["contract"]["methods"]] == methods, name
    raw = bytes(int(byte, 16) for byte in re.findall(r"0x([0-9a-f]{2})", (directory / "include" / header).read_text()))
    assert str(uuid.UUID(bytes=raw)) == entry["id"], name
    assert f"UUID {name.rsplit('::', 1)[-1]} {entry['id']}" in log, name
    if directory != args.app:
        installed = [p for p in [args.sdk / "include" / header, args.sdk / "include/dali3" / header] if p.exists()]
        assert len(installed) == 1 and installed[0].read_bytes() == (directory / "include" / header).read_bytes(), name
    print("PASS manifest/header/runtime UUID and ordered methods:", name)
assert len({entry["id"] for entry in entries.values()}) == 3

maps = []
for directory in [args.core, args.ui, args.app]:
    for provider in json.loads((directory / "providers.json").read_text())["providers"]:
        assert provider["id"] == entries[provider["interface"]]["id"]
        maps.append((provider["implementation"], provider["interface"]))
assert sorted(maps) == sorted([
    ("Dali::Internal::Animation", "Dali::IPlayback"),
    ("Dali::Ui::Integration::LabelImpl", "Dali::Ui::ITextContent"),
    ("QueryInterfaceSample::SampleCardImpl", "Dali::Ui::ITextContent"),
    ("QueryInterfaceSample::SampleCardImpl", "QueryInterfaceSample::IResettable"),
    ("QueryInterfaceSample::DerivedCardImpl", "Dali::Ui::ITextContent"),
    ("QueryInterfaceSample::DerivedCardImpl", "QueryInterfaceSample::IResettable"),
])
print("PASS exact generated provider maps (6) and imported UUID reuse")

source = '''#include <dali/public-api/object/query-interface.h>
struct Ordinary { virtual void Call() = 0; };
void test(const Dali::BaseHandle& handle) { Dali::QueryInterface<Ordinary>(handle); }
'''
result = subprocess.run(["c++", "-std=c++17", "-fsyntax-only", "-x", "c++", "-",
                         "-I" + str(args.sdk / "include/dali3"),
                         "-I" + str(args.sdk / "include")], input=source, text=True, capture_output=True)
assert result.returncode != 0 and "QueryInterface requires a generated DALI_QUERY_INTERFACE contract" in result.stderr
print("PASS untagged interface is rejected at compile time")
assert "SELF-TEST PASSED" in log
