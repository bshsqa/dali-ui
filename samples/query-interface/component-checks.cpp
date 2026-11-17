/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
// EXTERNAL INCLUDES
#include <cstdio>
#include <stdexcept>
#include <utility>

// INTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include "component-checks.h"
#include "sample-card.h"

namespace QueryInterfaceSample
{
using namespace Dali;
using namespace Dali::Ui;

unsigned RunComponentChecks()
{
  unsigned passed = 0;
  auto     check  = [&passed](bool condition, const char* name)
  {
    if(!condition) throw std::runtime_error(name);
    ++passed;
    std::printf("PASS %s\n", name);
  };
  auto printId = [](const char* name, const InterfaceId& id)
  {
    std::printf("UUID %s ", name);
    for(unsigned i = 0; i != 16; ++i)
    {
      if(i == 4 || i == 6 || i == 8 || i == 10) std::printf("-");
      std::printf("%02x", id.bytes[i]);
    }
    std::printf("\n");
  };
  printId("IPlayback", InterfaceTraits<IPlayback>::ID);
  printId("ITextContent", InterfaceTraits<ITextContent>::ID);
  printId("IResettable", InterfaceTraits<IResettable>::ID);
  check(InterfaceTraits<IPlayback>::ID != InterfaceTraits<ITextContent>::ID &&
          InterfaceTraits<ITextContent>::ID != InterfaceTraits<IResettable>::ID,
        "separate contracts have separate UUIDs");
  check(!QueryInterface<IPlayback>(BaseHandle()), "empty handle returns empty interface");
  auto       actor     = Actor::New();
  const auto actorRefs = actor.GetBaseObject().ReferenceCount();
  check(!QueryInterface<IPlayback>(actor), "unsupported Actor interface returns empty");
  check(actorRefs == actor.GetBaseObject().ReferenceCount(), "failed query does not retain owner");

  auto       animation = Animation::New(1.0f);
  const auto refs      = animation.GetBaseObject().ReferenceCount();
  auto       playback  = QueryInterface<IPlayback>(animation);
  check(bool(playback), "core Animation exposes IPlayback through BaseHandle");
  check(animation.GetBaseObject().ReferenceCount() == refs + 1, "query retains exactly one owner reference");
  playback->Play();
  check(animation.GetState() == Animation::PLAYING, "IPlayback.Play dispatches to Animation");
  playback->Pause();
  check(animation.GetState() == Animation::PAUSED, "IPlayback.Pause dispatches to Animation");
  playback->Play();
  check(animation.GetState() == Animation::PLAYING, "IPlayback.Play resumes Animation");
  playback->Stop();
  check(animation.GetState() == Animation::STOPPED, "IPlayback.Stop dispatches to Animation");
  check(playback.Get() == QueryInterface<IPlayback>(animation).Get(), "repeated query returns identical interface pointer");
  check(!playback.As<ITextContent>(), "cross-query of unsupported contract is empty");

  auto label = Label::New("before");
  auto text  = QueryInterface<ITextContent>(label);
  check(bool(text), "UI Label exposes ITextContent");
  text->SetText("QueryInterface: 한글 UTF-8");
  check(label.GetText() == String("QueryInterface: 한글 UTF-8"), "ITextContent.SetText updates actual Label");
  label.SetText("through existing API");
  check(text->GetText() == String("through existing API"), "ITextContent.GetText observes existing Label API");
  text->SetText("");
  check(text->GetText().Empty(), "empty text round trips");
  auto constText = QueryInterface<const ITextContent>(label);
  check(constText->GetText().Empty(), "const interface uses the same contract");
  constText.Reset();
  WeakHandleBase labelWeak(label);
  label.Reset();
  text->SetText("retained");
  check(text->GetText() == String("retained"), "interface remains usable after original Label handle reset");
  text.Reset();
  check(!labelWeak.GetBaseHandle(), "last interface releases Label owner");

  {
    IntrusivePtr<DerivedCardImpl> derived(new DerivedCardImpl());
    View                          inherited(*derived);
    derived->Initialize();
    auto inheritedText  = QueryInterface<ITextContent>(inherited);
    auto inheritedReset = QueryInterface<IResettable>(inherited);
    check(inheritedText.Get() == static_cast<ITextContent*>(derived.Get()),
          "base and derived providers resolve the same inherited text subobject");
    check(inheritedReset.Get() == static_cast<IResettable*>(derived.Get()),
          "inherited second interface is mapped without registration code");
    inheritedText->SetText("derived implementation");
    check(inheritedText->GetText() == String("derived implementation"), "inherited interface methods dispatch correctly");
  }

  CallCounts     counts;
  auto           card = SampleCard::New(&counts);
  WeakHandleBase weak(card);
  auto           cardText = QueryInterface<ITextContent>(card);
  auto           reset    = QueryInterface<IResettable>(card);
  check(bool(cardText) && bool(reset), "unregistered app component exposes SDK and app contracts");
  check(static_cast<const void*>(cardText.Get()) != static_cast<const void*>(reset.Get()),
        "multiple inheritance resolves distinct interface subobjects");
  check(cardText.GetOwner() == reset.GetOwner(), "both interface subobjects retain the same owner");
  check(cardText.As<IResettable>().Get() == reset.Get(), "text to reset cross-query preserves identity");
  check(reset.As<ITextContent>().Get() == cardText.Get(), "reset to text cross-query preserves identity");
  cardText->SetText("app contract call");
  check(cardText->GetText() == String("app contract call"), "app SetText and GetText dispatch correctly");
  check(counts.sets == 1 && counts.gets == 1, "app SetText and GetText called exactly once");
  reset->ResetContent();
  check(cardText->GetText() == String("Custom component: reset") && counts.resets == 1,
        "app ResetContent dispatches exactly once and changes content");
  const auto cardRefs = card.GetBaseObject().ReferenceCount();
  auto       copy     = cardText;
  check(card.GetBaseObject().ReferenceCount() == cardRefs + 1, "copy retains one reference");
  auto moved = std::move(copy);
  check(!copy && !copy.GetOwner() && moved.Get() == cardText.Get(), "move empties source and preserves interface");
  check(card.GetBaseObject().ReferenceCount() == cardRefs + 1, "move does not add reference");
  InterfacePtr<ITextContent> assigned;
  assigned = moved;
  moved    = std::move(assigned);
  check(!assigned && bool(moved), "copy and move assignment maintain ownership");
  auto* self = &moved;
  moved      = *self;
  moved      = std::move(*self);
  check(moved.Get() == cardText.Get(), "self copy and self move remain valid");
  moved.Reset();
  check(card.GetBaseObject().ReferenceCount() == cardRefs, "temporary copies release their references");
  card.Reset();
  cardText->SetText("still alive");
  check(counts.destroyed == 0 && cardText->GetText() == String("still alive"), "interfaces retain app component after handle reset");
  cardText.Reset();
  check(counts.destroyed == 0, "second interface independently retains owner");
  reset->ResetContent();
  reset.Reset();
  check(counts.destroyed == 1 && !weak.GetBaseHandle(), "last interface destroys app component exactly once");
  std::printf("FUNCTIONAL CHECKS: %u passed\n", passed);
  return passed;
}

} //namespace QueryInterfaceSample
