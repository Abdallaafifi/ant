#pragma once

#include <core/Layout.h>
#include <core/ComputedValues.h>
#include <core/ObserverPtr.h>
#include <core/Property.h>
#include <core/Types.h>
#include <core/Tween.h>
#include <core/Geometry.h>
#include <core/Node.h>
#include <core/PropertyIdSet.h>
#include <core/PropertyVector.h>
#include <core/StyleCache.h>
#include <optional>
#include "luavalue.h"

namespace Rml {

class DataModel;
class Document;
class Element;
class ElementAnimation;
class ElementTransition;
class EventListener;
class Geometry;
class StyleSheet;
struct HtmlElement;

using ElementList = std::vector<Element*>;
using ElementAttributes = std::unordered_map<std::string, std::string>;

class Element : public Node, public EnableObserverPtr<Element> {
public:
	Element(Document* owner, const std::string& tag);
	virtual ~Element();

	const StyleSheet& GetStyleSheet() const;
	std::string GetAddress(bool include_pseudo_classes = false, bool include_parents = true) const;
	bool IgnorePointerEvents() const;

	bool IsRemoved() const;
	float GetOpacity();
	bool IsGray();
	float GetFontSize() const;
	bool UpdataFontSize();

	void SetAttribute(const std::string& name, const std::string& value);
	const std::string* GetAttribute(const std::string& name) const;
	bool HasAttribute(const std::string& name) const;
	void RemoveAttribute(const std::string& name);
	const ElementAttributes& GetAttributes() const { return attributes; }

	bool Project(Point& point) const noexcept;
	const std::string& GetTagName() const;
	const std::string& GetId() const;
	void SetId(const std::string& id);
	Document* GetOwnerDocument() const;

	void InstanceOuter(const HtmlElement& html);
	void InstanceInner(const HtmlElement& html);
	void NotifyCustomElement();

	void AddEventListener(EventListener* listener);
	void RemoveEventListener(EventListener* listener);
	void RemoveEventListener(const std::string& type);
	bool DispatchEvent(const std::string& type, int parameters_ref, bool interruptible, bool bubbles);
	bool DispatchEvent(const std::string& type, const luavalue::table& parameters, bool interruptible, bool bubbles);
	bool DispatchAnimationEvent(const std::string& type, const ElementAnimation& animation);
	void RemoveAllEvents();
	const std::vector<std::unique_ptr<EventListener>>& GetEventListeners() const;

	void   AppendChild(Node* node, uint32_t index = 0xffffffff);
	void   RemoveChild(Node* node);
	std::unique_ptr<Node> DetachChild(Node* node);
	size_t GetChildNodeIndex(Node* node) const;
	void   InsertBefore(Node* child, Node* adjacent);
	Node*  GetPreviousSibling();
	void   RemoveAllChildren();

	auto const& Children() const { return children; }
	auto const& ChildNodes() const { return childnodes; }
	auto&       Children() { return children; }
	auto&       ChildNodes() { return childnodes; }

	Node* GetChildNode(size_t index) const;
	size_t GetNumChildNodes() const;

	Element* GetElementById(const std::string& id);
	void GetElementsByTagName(ElementList& elements, const std::string& tag);
	void GetElementsByClassName(ElementList& elements, const std::string& class_name);

	void Update();
	void UpdateRender();
	void SetRenderStatus();

	Size GetScrollOffset() const;
	float GetScrollLeft() const;
	float GetScrollTop() const;
	void SetScrollLeft(float v);
	void SetScrollTop(float v);
	void SetScrollInsets(const EdgeInsets<float>& insets);
	void UpdateScrollOffset(Size& scrollOffset) const;

	void SetPseudoClass(PseudoClass pseudo_class, bool activate);
	bool IsPseudoClassSet(PseudoClassSet pseudo_class) const;
	PseudoClassSet GetActivePseudoClasses() const;
	void SetClass(const std::string& class_name, bool activate);
	bool IsClassSet(const std::string& class_name) const;
	void SetClassName(const std::string& class_names);
	void DirtyPropertiesWithUnitRecursive(PropertyUnit unit);

	void UpdateDefinition();
	void DirtyDefinition();
	void DirtyInheritableProperties();
	void DirtyProperty(PropertyId id);
	void DirtyProperties(const PropertyIdSet& properties);
	void DirtyProperties(PropertyUnit unit);

	bool SetProperty(const PropertyVector& vec);
	bool DelProperty(const PropertyIdSet& set);
	void SetAnimationProperty(PropertyId id, const Property& property);
	void DelAnimationProperty(PropertyId id);

	std::optional<Property> GetInlineProperty(PropertyId id) const;
	std::optional<Property> GetLocalProperty(PropertyId id) const;
	std::optional<Property> GetComputedProperty(PropertyId id) const;
	template <typename T>
	auto GetProperty(PropertyId id) const {
		if constexpr(std::is_same_v<T, float>) {
			return GetComputedProperty(id)->Get<T>(this);
		}
		else {
			return GetComputedProperty(id)->Get<T>();
		}
	}

	bool SetProperty(const std::string& name, std::optional<std::string> value = std::nullopt);
	std::optional<std::string> GetProperty(const std::string& name) const;

	void UpdateProperties();
	void UpdateAnimations(float delta);

	const EdgeInsets<float>& GetPadding() const;
	const EdgeInsets<float>& GetBorder() const;

	void SetParentNode(Element* parent) override;
	void UpdateDataModel() override;
	DataModel* GetDataModel() const override;
	void DataModelLoad(const std::string& name, const std::string& value);
	void DataModelSetVariable(const std::string& name, const std::string& value);
	Node* Clone(bool deep = true) const override;
	void CalculateLayout() override;
	void Render() override;
	float GetZIndex() const override;
	Element* ElementFromPoint(Point point) override;
	Element* ChildFromPoint(Point point);
	std::string GetInnerHTML() const override;
	std::string GetOuterHTML() const override;
	void SetInnerHTML(const std::string& html) override;
	void SetOuterHTML(const std::string& html) override;
	const Rect& GetContentRect() const override;
	void ChangedProperties(const PropertyIdSet& changed_properties);
	void DirtyImage();

protected:
	void OnAttributeChange(const ElementAttributes& changed_attributes);
	void UpdateStackingContext();
	void DirtyStackingContext();
	void DirtyStructure();
	void DirtyDataModel() override;
	void UpdateStructure();
	void DirtyPerspective();
	void UpdateTransform();
	void UpdatePerspective();
	void UpdateGeometry();
	void DirtyTransform();
	void DirtyClip();
	void UpdateClip();
	bool SetInlineProperty(const PropertyVector& vec);
	bool DelInlineProperty(const PropertyIdSet& set);
	void RefreshProperties();

	void StartTransition(std::function<void()> f);
	void HandleTransitionProperty();
	void HandleAnimationProperty();
	void AdvanceAnimations(float delta);

	std::string tag;
	std::string id;
	Document* owner_document;
	ElementAttributes attributes;
	std::vector<Element*> children;
	std::vector<std::unique_ptr<Node>> childnodes;
	std::vector<Node*> stacking_context;
	std::unique_ptr<glm::mat4x4> perspective;
	mutable bool have_inv_transform = true;
	mutable std::unique_ptr<glm::mat4x4> inv_transform;
	std::map<PropertyId, ElementAnimation> animations;
	std::map<PropertyId, ElementTransition> transitions;
	std::vector<std::string> classes;
	PseudoClassSet pseudo_classes = 0;
	std::vector<std::unique_ptr<EventListener>> listeners;
	std::unique_ptr<Geometry> geometry_background;
	std::unique_ptr<Geometry> geometry_image;
	float font_size = 16.f;
	Style::Value animation_properties = Style::Instance().Create();
	Style::Value inline_properties = Style::Instance().Create();
	Style::Value definition_properties = Style::Instance().Create();
	Style::Combination local_properties = Style::Instance().Merge(animation_properties, inline_properties, definition_properties);
	Style::Combination global_properties = Style::Instance().Inherit(local_properties);
	PropertyIdSet dirty_properties;
	glm::mat4x4 transform;
	Rect content_rect;
	EdgeInsets<float> padding{};
	EdgeInsets<float> border{};
	EdgeInsets<float> scroll_insets{};
	Geometry::Path padding_edge;
	struct Clip {
		enum class Type : uint8_t {
			None,
			Scissor,
			Shader,
		} type = Type::None;
		union {
			glm::u16vec4 scissor;
			glm::vec4 shader[2];
		};
	} clip;
	void UnionClip(Clip& clip);

	enum class Dirty {
		Transform,
		Clip,
		StackingContext,
		Structure,
		Perspective,
		Animation,
		Transition,
		Background,
		Image,
		Definition,
		DataModel,
	};
	EnumSet<Dirty> dirty;
};

}
