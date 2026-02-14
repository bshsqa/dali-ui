#pragma once

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
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/common/extents.h>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <memory>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/focus/focus-direction.h>
#include <dali-ui-foundation/public-api/trait.h>
#include <dali-ui-foundation/public-api/layout-types.h>
#include <dali-ui-foundation/integration-api/trait-id.h>

namespace Dali
{

namespace UI
{

// Forward declarations
class Layout;
class LayoutManager;

namespace Integration
{

/**
 * @brief This is the internal implementation class for View.
 *
 * ViewImpl extends CustomActorImpl to provide basic UI functionality.
 * It implements the p-impl pattern and provides the actual implementation
 * for the View public API.
 *
 * View can optionally have a LayoutManager to manage child layout.
 * When a LayoutManager is set, the View can manage children and delegate
 * layout calculations to the LayoutManager.
 *
 * @see Dali::UI::View
 */
class DALI_UI_API ViewImpl : public CustomActorImpl
{
public:

  // Creation & Destruction

  /**
   * @brief Child data structure for layout calculations.
   *
   * Stores information about each child including the View handle,
   * measured size, and arranged bounds.
   */
  struct ChildData
  {
    UI::View view;             ///< Handle to the child view
    MeasuredSize measuredSize; ///< Size from Measure pass
    LayoutRect arrangedBounds; ///< Bounds from Arrange pass
  };

  /**
   * @brief Container type for children.
   */
  using ChildContainer = std::vector<ChildData>;

  /**
   * @brief Creates a new View.
   */
  static UI::View New();

protected:

  /**
   * @brief Destructor.
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ViewImpl();

  // Construction

  /**
   * @brief View constructor.
   */
  ViewImpl();

public: // From CustomActorImpl
  /**
   * @copydoc CustomActorImpl::OnSceneConnection
   * When this View is a layout root and is connected to a window, registers
   * with LayoutController so that measure/arrange runs even if invalidation
   * occurred before the view was added to the window.
   */
  void OnSceneConnection(int32_t depth) override;

  /**
   * @copydoc CustomActorImpl::OnSceneDisconnection
   */
  void OnSceneDisconnection() override;

  /**
   * @copydoc CustomActorImpl::OnChildAdd
   */
  void OnChildAdd(Actor& child) override;

  /**
   * @copydoc CustomActorImpl::OnChildRemove
   */
  void OnChildRemove(Actor& child) override;

  /**
   * @copydoc CustomActorImpl::OnPropertySet
   */
  void OnPropertySet(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * @copydoc CustomActorImpl::OnSizeSet
   */
  void OnSizeSet(const Vector3& targetSize) override;

  /**
   * @copydoc CustomActorImpl::OnSizeAnimation
   */
  void OnSizeAnimation(Animation& animation, const Vector3& targetSize) override;

  /**
   * @copydoc CustomActorImpl::GetOffScreenRenderTasks
   */
  void GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward) override;

  /**
   * @copydoc CustomActorImpl::OnRelayout
   * Override to separate dali-ui layout from DALi size negotiation.
   * When this View has a LayoutManager, size/position are driven by dali-ui
   * LayoutController; we no-op. Otherwise delegate to base logic if needed.
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc CustomActorImpl::OnSetResizePolicy
   */
  void OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::GetNaturalSize
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc CustomActorImpl::CalculateChildSize
   */
  float CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::GetHeightForWidth
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc CustomActorImpl::GetWidthForHeight
   */
  float GetWidthForHeight(float height) override;

  /**
   * @copydoc CustomActorImpl::RelayoutDependentOnChildren
   */
  bool RelayoutDependentOnChildren(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) override;

  /**
   * @copydoc CustomActorImpl::OnCalculateRelayoutSize
   */
  void OnCalculateRelayoutSize(Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::OnLayoutNegotiated
   */
  void OnLayoutNegotiated(float size, Dimension::Type dimension) override;

public: // Validation Logic (Migrated from Control)
  /**
   * @brief Initializes the View.
   */
  void Initialize();

  /**
   * @brief Called when the view gains key input focus.
   */
  void OnKeyInputFocusGained();

  /**
   * @brief Called when the view loses key input focus.
   */
  void OnKeyInputFocusLost();

  /**
   * @brief Emits the key event signal.
   * @param[in] event The key event.
   * @return True if consumed.
   */
  bool EmitKeyEventSignal(const KeyEvent& event);

  // Keyboard Navigation

  /**
   * @brief Sets whether this view supports 2D keyboard navigation.
   * @param[in] isSupported Whether this view supports 2D keyboard navigation.
   */
  void SetKeyboardNavigationSupport(bool isSupported);

  /**
   * @brief Gets whether this view supports 2D keyboard navigation.
   * @return true if supported.
   */
  bool IsKeyboardNavigationSupported() const;

  /**
   * @brief Sets whether this view is a keyboard focus group.
   * @param[in] isFocusGroup Whether this view is a focus group.
   */
  void SetAsKeyboardFocusGroup(bool isFocusGroup);

  /**
   * @brief Gets whether this view is a keyboard focus group.
   * @return true if a focus group.
   */
  bool IsKeyboardFocusGroup() const;

  /**
   * @brief Gets the next keyboard focusable actor in this view towards the given direction.
   *
   * A view needs to override this function to support 2D keyboard navigation.
   * @param[in] currentFocusedActor The current focused actor.
   * @param[in] direction The direction to move the focus towards.
   * @param[in] loopEnabled Whether the focus movement should be looped.
   * @return The next keyboard focusable actor or an empty handle.
   */
  virtual Actor GetNextKeyboardFocusableActor(Actor currentFocusedActor, UI::Focus::Direction direction, bool loopEnabled);

  /**
   * @brief Informs this view that its chosen focusable actor will be focused.
   * @param[in] committedFocusableActor The committed focusable actor.
   */
  virtual void OnKeyboardFocusChangeCommitted(Actor committedFocusableActor);

  /**
   * @brief Called when the view has enter pressed on it.
   * @return true if this view handled the enter key.
   */
  virtual bool OnKeyboardEnter();

  /**
   * @brief Called after a key-event is received by the actor that has had its focus set.
   * @param[in] event The Key Event.
   * @return True if the event should be consumed.
   */
  virtual bool OnKeyEvent(const KeyEvent& event);

public: // Signals
  /**
   * @copydoc Dali::UI::View::KeyEventSignal
   */
  UI::View::KeyEventSignalType& KeyEventSignal();

  /**
   * @copydoc Dali::UI::View::KeyInputFocusGainedSignal
   */
  UI::View::KeyInputFocusSignalType& KeyInputFocusGainedSignal();

  /**
   * @copydoc Dali::UI::View::KeyInputFocusLostSignal
   */
  UI::View::KeyInputFocusSignalType& KeyInputFocusLostSignal();


public: // API (size, position, parent origin, pivot)
  /**
   * @copydoc Dali::UI::View::GetSizeWidth
   */
  float GetSizeWidth() const;

  /**
   * @copydoc Dali::UI::View::SetSizeWidth
   */
  void SetSizeWidth(float width);

  /**
   * @copydoc Dali::UI::View::GetSizeHeight
   */
  float GetSizeHeight() const;

  /**
   * @copydoc Dali::UI::View::SetSizeHeight
   */
  void SetSizeHeight(float height);

  /**
   * @copydoc Dali::UI::View::GetPositionX
   */
  float GetPositionX() const;

  /**
   * @copydoc Dali::UI::View::SetPositionX
   */
  void SetPositionX(float x);

  /**
   * @copydoc Dali::UI::View::GetPositionY
   */
  float GetPositionY() const;

  /**
   * @copydoc Dali::UI::View::SetPositionY
   */
  void SetPositionY(float y);

  /**
   * @copydoc Dali::UI::View::GetParentOrigin
   */
  Vector3 GetParentOrigin() const;

  /**
   * @copydoc Dali::UI::View::SetParentOrigin
   */
  void SetParentOrigin(const Vector3& point);

  /**
   * @copydoc Dali::UI::View::GetPivotPoint
   */
  Vector3 GetPivotPoint() const;

  /**
   * @copydoc Dali::UI::View::SetPivotPoint
   */
  void SetPivotPoint(const Vector3& point);

  /**
   * @brief Sets a trait to this View.
   *
   * The trait will share the lifecycle with this View.
   *
   * @throws DaliException If the trait already has an owner
   * @throws DaliException If the id is already registered with other trait
   *
   * @note **Strong Reference**
   * The view will hold the strong reference to the trait after attached.
   *
   * @param[in] id The unique key to identify the trait
   * @param[in] trait The trait object to attach
   */
  void SetTrait(TraitId id, Trait& trait);

  /**
   * @brief Gets a trait from this View.
   * @param[in] id The unique key to identify the trait
   * @return The trait handle
   */
  Trait GetTrait(TraitId id) const;

  /**
   * @brief Removes a trait from this View.
   * @param[in] id The unique key to identify the trait
   * @return True if succeeded, false otherwise
   */
  bool RemoveTrait(TraitId id);

public: // Measure / Arrange API
  /**
   * @brief Measures the view with the given constraints.
   */
  MeasuredSize Measure(float widthConstraint, float heightConstraint);

  /**
   * @brief Arranges the view within the given bounds.
   */
  MeasuredSize Arrange(const LayoutRect& bounds);

  /**
   * @brief Invalidates the measure of this view and propagates up.
   */
  void InvalidateMeasure();

  /**
   * @brief Invalidates the arrange of this view.
   */
  void InvalidateArrange();

  /**
   * @brief Gets the desired size after measurement.
   */
  MeasuredSize GetDesiredSize() const;

  /**
   * @brief Checks if the measure is valid.
   */
  bool IsMeasureValid() const;

  /**
   * @brief Checks if the arrange is valid.
   */
  bool IsArrangeValid() const;

protected: // Virtual methods for derived classes (Template Method pattern)
  /**
   * @brief Called during measure pass.
   */
  virtual MeasuredSize OnMeasure(float widthConstraint, float heightConstraint);

  /**
   * @brief Called during arrange pass.
   */
  virtual MeasuredSize OnArrange(const LayoutRect& bounds);

public: // Layout size API (LayoutWidth / LayoutHeight)
  void SetLayoutWidth(float width);
  float GetLayoutWidth() const;
  void SetLayoutHeight(float height);
  float GetLayoutHeight() const;
  void SetMinimumWidth(float width);
  float GetMinimumWidth() const;
  void SetMinimumHeight(float height);
  float GetMinimumHeight() const;
  void SetMaximumWidth(float width);
  float GetMaximumWidth() const;
  void SetMaximumHeight(float height);
  float GetMaximumHeight() const;

public: // Layout Properties API
  void SetViewMargin(const Extents& margin);
  Extents GetViewMargin() const;
  void SetViewPadding(const Extents& padding);
  Extents GetViewPadding() const;
  void SetHorizontalAlignment(LayoutAlignment alignment);
  LayoutAlignment GetHorizontalAlignment() const;
  void SetVerticalAlignment(LayoutAlignment alignment);
  LayoutAlignment GetVerticalAlignment() const;

public: // Parent Layout API
  UI::Layout GetParentLayout() const;
  virtual bool IsLayout() const;

public: // LayoutManager API (Optional layout capability)
  /**
   * @brief Sets the layout manager for this view.
   *
   * When a LayoutManager is set, the view can manage children and
   * will delegate OnMeasure/OnArrange to the LayoutManager.
   *
   * @param[in] layoutManager The layout manager (ownership transferred)
   */
  void SetLayoutManager(LayoutManager* layoutManager);

  /**
   * @brief Gets the layout manager.
   *
   * @return Pointer to the layout manager, or nullptr if not set
   */
  LayoutManager* GetLayoutManager() const;

  /**
   * @brief Checks if this view has a layout manager.
   *
   * @return True if a layout manager is set
   */
  bool HasLayoutManager() const;

public: // Child Management API (available when LayoutManager is set)
  /**
   * @brief Adds a view to this view (at the end).
   *
   * The view will be managed by this view's layout manager and added to the Actor hierarchy.
   *
   * @param[in] view The view to add
   */
  void AddView(UI::View view);

  /**
   * @brief Adds a view at the specified index.
   *
   * @param[in] view The view to add
   * @param[in] index The index where to insert
   */
  void AddView(UI::View view, uint32_t index);

  /**
   * @brief Removes a view from this view.
   *
   * @param[in] view The view to remove
   */
  void RemoveView(UI::View view);

  /**
   * @brief Removes the view at the specified index.
   *
   * @param[in] index The index of the view to remove
   */
  void RemoveViewAt(uint32_t index);

  /**
   * @brief Removes all child views from this view.
   */
  void RemoveAllViews();

  /**
   * @brief Gets the number of child views.
   *
   * @return The child count
   */
  uint32_t GetChildCount() const;

  /**
   * @brief Gets the child view at the specified index.
   *
   * @param[in] index The child index
   * @return The child view at the index
   */
  UI::View GetChildAt(uint32_t index) const;

  /**
   * @brief Returns the index of the given child view, or -1 if not found.
   *
   * @param[in] view The child view to find
   * @return Index of the view, or -1 if not a child
   */
  int32_t IndexOfChild(UI::View view) const;

  /**
   * @brief Adds a list of children (method chaining).
   *
   * @param[in] children The initializer list of View handles to add
   * @return Reference to this for method chaining
   */
  ViewImpl& Contents(std::initializer_list<UI::View> children);

  /**
   * @brief Gets the children container for layout manager access.
   *
   * @return Reference to the children container
   */
  ChildContainer& GetChildren();

  /**
   * @brief Gets the children container (const version).
   *
   * @return Const reference to the children container
   */
  const ChildContainer& GetChildren() const;

public: // ClipsToBounds API
  /**
   * @brief Sets whether this view clips its children to its bounds.
   *
   * @param[in] clips True to clip children to bounds
   */
  void SetClipsToBounds(bool clips);

  /**
   * @brief Gets whether this view clips its children to its bounds.
   *
   * @return True if children are clipped to bounds
   */
  bool GetClipsToBounds() const;

protected:
  /**
   * @brief Applies min/max constraints to the size.
   */
  MeasuredSize ApplyConstraints(const MeasuredSize& size) const;

  /**
   * @brief Registers this layout with the LayoutController for processing.
   *
   * Called when this view is a Layout Root (top of layout hierarchy).
   */
  void RegisterWithLayoutController();

private:

  // Not copyable or movable
  ViewImpl(const ViewImpl&) = delete;
  ViewImpl(ViewImpl&&) = delete;
  ViewImpl& operator=(const ViewImpl&) = delete;
  ViewImpl& operator=(ViewImpl&&) = delete;

private:

  std::vector<std::pair<TraitId, Trait>> mTraits;

  // Layout size (LayoutWidth / LayoutHeight)
  float mLayoutWidth;
  float mLayoutHeight;
  float mMinimumWidth;
  float mMinimumHeight;
  float mMaximumWidth;
  float mMaximumHeight;

  // Layout Properties Data
  Extents mMargin;
  Extents mPadding;
  LayoutAlignment mHorizontalAlignment;
  LayoutAlignment mVerticalAlignment;

  // Measure/Arrange State (cache-based)
  // mLastMeasuredConstraint.width < 0 means no valid measure cache
  MeasuredSize mDesiredSize;
  MeasuredSize mLastMeasuredConstraint;
  LayoutRect mArrangedBounds;
  bool mArrangeValid;

  // Optional LayoutManager and Children (for layout capability)
  std::unique_ptr<LayoutManager> mLayoutManager;
  ChildContainer mChildren;

  // ClipsToBounds (layout-style behaviour when view has children)
  bool mClipsToBounds;

  // Keyboard Navigation
  bool mIsKeyboardNavigationSupported;
  bool mIsKeyboardFocusGroup;

  // Signals
  UI::View::KeyEventSignalType      mKeyEventSignal;
  UI::View::KeyInputFocusSignalType mKeyInputFocusGainedSignal;
  UI::View::KeyInputFocusSignalType mKeyInputFocusLostSignal;
};

// Helpers for public-api forwarding methods

inline Integration::ViewImpl& GetImpl(UI::View& view)
{
  DALI_ASSERT_ALWAYS(view);

  Dali::RefObject& handle = view.GetImplementation();

  return static_cast<Integration::ViewImpl&>(handle);
}

inline const Integration::ViewImpl& GetImpl(const UI::View& view)
{
  DALI_ASSERT_ALWAYS(view);

  const Dali::RefObject& handle = view.GetImplementation();

  return static_cast<const Integration::ViewImpl&>(handle);
}

} // namespace Integration

} // namespace UI

} // namespace Dali
