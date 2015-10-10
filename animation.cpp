#include "animation.h"
#include <set>
#include "ogl/render.h"
#include "mainwindow.h"
#include "extmath.h"
#include "rapidxml/rapidxml_print.hpp"

Animation *Animation::_instance = NULL;

Animation::Animation()
{
    assert(_instance == NULL);
    _instance = this;
    _boneMoving = false;
    _startMovingBone = -1;
    _texture = NULL;
    _baseSprite = NULL;
    _meshGenerateBone = -1;
}

Animation::~Animation()
{
    assert(_instance != NULL);
    _instance = NULL;

    if (_texture)
        delete _texture;

    if (_baseSprite)
        delete _baseSprite;
}

void Animation::Draw()
{
    if (MainWindow::Instance()->CreateDotMode())
    {
        _bones = &_bonesOrigin;
    }
    else
    {
        _bones = &_bonesAnimation;
    }

    if (_baseSprite && MainWindow::Instance()->CreateDotMode())
    {
        Render::SetColor(0x7FFFFFFF);
        _baseSprite->Render(- _baseSprite->Width() / 2, - _baseSprite->Height() / 2);
        Render::SetColor(0xFFFFFFFF);
    }

    // hide all bones
    for (BoneList::iterator i = _bones->begin(), e = _bones->end(); i != e; ++i)
    {
        (*i)->SetVisible(false);
    }

    // calculate new positions and visibility
    Matrix pos;
    pos.Unit();
    for (BoneList::iterator i = _bones->begin(), e = _bones->end(); i != e; ++i)
    {
        if ((*i)->GetParent() == NULL)
            (*i)->CalculatePosition(pos, 0.f);
    }

    // draw meshes
    if (MainWindow::Instance()->CreateDotMode())
    {
        for (uint i = 0; i < _meshes.size(); ++i)
        {
            _meshes[i]->Draw();
        }
    }
    else
    {
        for (uint i = 0; i < _meshes.size(); ++i)
        {
            _meshes[i]->DrawBinded();
        }
    }

    // draw bones
    for (BoneList::iterator i = _bones->begin(), e = _bones->end(); i != e; ++i)
    {
        (*i)->Draw();
    }

    // draw selected bones
    for (std::set<uint>::iterator i = _selected.begin(); i != _selected.end(); ++i)
    {
        (*_bones)[*i]->DrawSelection();
    }

    // draw meshes
    if (MainWindow::Instance()->CreateDotMode() && _selected.size() == 1)
    {
        for (uint i = 0; i < _meshes.size(); ++i)
        {
            if (_meshes[i]->GetBone() == (*_bones)[*_selected.begin()]->GetName())
            {
                _meshes[i]->DebugDraw(true);
            }
        }
    }
}

std::string Animation::GenerateUnicBoneName()
{
    std::set<std::string> allNames;
    for (BoneList::iterator i = _bones->begin(), e = _bones->end(); i != e; ++i)
    {
        allNames.insert((*i)->GetName());
    }
    int i = 1;
    char name[100];
    do
    {
        sprintf(name, "bone_%03i", i++);
    } while (allNames.find(name) != allNames.end());
    return name;
}

uint Animation::CreateBone(FPoint pos, bool generateMesh)
{
    uint r = _bones->size();
    BoneAnimated *b = new BoneAnimated();
    b->SetBonePos(pos);
    b->SetName(GenerateUnicBoneName());
    _bones->push_back(b);

    if (generateMesh)
    {
        PointList points;

        FPoint p1(pos);
        FPoint p2 = *FPoint(b->GetLength(), 0.f).Rotate(b->GetBoneAngle()) + pos;
        FPoint o = *FPoint(0.f, b->GetLength() * 0.314f).Rotate(b->GetBoneAngle());

        points.push_back(p1 + o);
        points.push_back(p1 - o);
        points.push_back(p2 - o);
        points.push_back(p2 + o);

        ColoredPolygon *c = new ColoredPolygon(points);
        c->SetBone(b->GetName());
        _meshGenerateBone = _meshes.size();
        _meshes.push_back(c);
    }

    return r;
}

int Animation::GetBoneAtPoint(const FPoint &pos)
{
    for (uint i = 0; i < _bones->size(); ++i)
    {
        if ((*_bones)[i]->CheckPoint(pos))
        {
            BoneAnimated *b = (*_bones)[i]->GetBoneAtPoint(pos);
            if (b)
            {
                for (uint j = 0; j < _bones->size(); ++j)
                {
                    if ((*_bones)[j] == b)
                    {
                        return (int)j;
                    }
                }
            }
            return (int)i;
        }
    }
    return -1;
}

void Animation::StartBoneMoving(uint index, const FPoint &point)
{
    _boneMoving = (*_bones)[index]->MoveOrRotate(point);
    _startMovingPos = point;
    _startRotateAngle = (*_bones)[index]->GetBoneAngle();
    _startMovingBone = index;

    // -=IK=-

    //bone under cursor
    BoneAnimated *mb = (*_bones)[_startMovingBone];

    //find all selected parents
    _boneChain.clear();
    _boneChain.push_back(mb);

    std::set<BoneAnimated*> sel;
    for (std::set<uint>::iterator i = _selected.begin(); i != _selected.end(); ++i)
    {
        sel.insert((*_bones)[*i]);
    }

    BoneAnimated *parent = mb->GetParent();
    while (parent && sel.find(parent) != sel.end())
    {
        _boneChain.push_back(parent);
        parent = parent->GetParent();
    }

    for (uint i = 0; i < _boneChain.size() / 2; ++i)
    {
        BoneAnimated *tmp = _boneChain[i];
        _boneChain[i] = _boneChain[_boneChain.size() - 1 - i];
        _boneChain[_boneChain.size() - 1 - i] = tmp;
    }

    //form FPoint list
    _chainPoints.resize(_boneChain.size());
    for (uint i = 0; i < _boneChain.size(); ++i)
    {
        FPoint p(_boneChain[i]->GetBonePos());
        if (_boneChain[i]->GetParent())
        {
            _boneChain[i]->GetParent()->GetMatrix().Mul(p);
        }
        _chainPoints[i]= p;
    }
    _chainPoints.push_back(_startMovingPos);
    _deltaAngle.resize(_boneChain.size());
    for (uint i = 0; i < _deltaAngle.size(); ++i)
    {
        FPoint a(_chainPoints[i]);
        FPoint b(_chainPoints[i + 1]);

        Matrix rev;
        rev.MakeRevers(_boneChain[i]->GetMatrix());

        rev.Mul(a);
        rev.Mul(b);

        _deltaAngle[i] = - atan2f(b.y - a.y, b.x - a.x);
    }

}

void Animation::BoneMoveTo(const FPoint &point, bool changeLength)
{
    if (_boneMoving || _selected.size() > 1)
    {
        for (std::set<uint>::iterator i = _selected.begin(); i != _selected.end(); ++i)
        {
            if ((*_bones)[*i]->GetMoveByParent()) continue;

            if ((*_bones)[*i]->GetParent() == NULL)
                (*_bones)[*i]->MoveTo(point - _startMovingPos);
            else
            {
                FPoint p(point);
                FPoint s(_startMovingPos);

                Matrix rev;
                rev.MakeRevers((*_bones)[*i]->GetParent()->GetMatrix());

                rev.Mul(p);
                rev.Mul(s);

                (*_bones)[*i]->MoveTo(p - s);
            }
        }
        _startMovingPos = point;

        if (_selected.size() == 1)
        {
            for (uint i = 0; i < _bones->size(); ++i)
            {
                FPoint p((*_bones)[*_selected.begin()]->GetBonePos());
                if ((*_bones)[*_selected.begin()]->GetParent())
                {
                    (*_bones)[*_selected.begin()]->GetParent()->GetMatrix().Mul(p);
                }

                if (i != *_selected.begin() && (*_bones)[i]->CheckPoint(p))
                {
                    LinkBones(i, *_selected.begin());
                    return;
                }
            }
        }
    }
    else if (_selected.size() == 1)
    {
        FPoint s((*_bones)[*_selected.begin()]->GetBonePos());
        FPoint e(point);

        if ((*_bones)[*_selected.begin()]->GetParent() == NULL)
            (*_bones)[*_selected.begin()]->SetBoneAngle(_startRotateAngle
                                        + (atan2(point.y - (*_bones)[*_selected.begin()]->GetBonePos().y, point.x - (*_bones)[*_selected.begin()]->GetBonePos().x)
                                            - atan2(_startMovingPos.y - (*_bones)[*_selected.begin()]->GetBonePos().y, _startMovingPos.x - (*_bones)[*_selected.begin()]->GetBonePos().x)));
        else
        {
            FPoint o((*_bones)[*_selected.begin()]->GetBonePos());
            FPoint sm(_startMovingPos);
            Matrix rev;
            rev.MakeRevers((*_bones)[*_selected.begin()]->GetParent()->GetMatrix());
            rev.Mul(e);
            rev.Mul(sm);

            (*_bones)[*_selected.begin()]->SetBoneAngle(_startRotateAngle + (atan2(e.y - o.y, e.x - o.x)
                                                                    - atan2(sm.y - o.y, sm.x - o.x)));

        }
        if (changeLength)
        {
            (*_bones)[*_selected.begin()]->SetLength((s - e).Length());
        }
        if (_meshGenerateBone >= 0)
        {
            PointList points;
            BoneAnimated *b = (*_bones)[*_selected.begin()];

            FPoint p1(b->GetBonePos());
            FPoint p2 = *FPoint(b->GetLength(), 0.f).Rotate(b->GetBoneAngle()) + b->GetBonePos();
            FPoint o = *FPoint(0.f, b->GetLength() * 0.314f).Rotate(b->GetBoneAngle());

            points.push_back(p1 + o);
            points.push_back(p1 - o);
            points.push_back(p2 - o);
            points.push_back(p2 + o);

            _meshes[_meshGenerateBone]->UpdatePoints(points);
        }
    }
}

void Animation::BoneCreateTo(const FPoint &point)
{
    FPoint s((*_bones)[*_selected.begin()]->GetBonePos());
    FPoint e(point);

    if ((*_bones)[*_selected.begin()]->GetParent() != NULL)
    {
        Matrix rev;
        rev.MakeRevers((*_bones)[*_selected.begin()]->GetParent()->GetMatrix());
        rev.Mul(e);
    }
    (*_bones)[*_selected.begin()]->SetBoneAngle(atan2(e.y - s.y, e.x - s.x));
    (*_bones)[*_selected.begin()]->SetLength((s - e).Length());

    if (_meshGenerateBone >= 0)
    {
        PointList points;
        BoneAnimated *b = (*_bones)[*_selected.begin()];

        FPoint p1(b->GetBonePos());
        FPoint p2 = *FPoint(b->GetLength(), 0.f).Rotate(b->GetBoneAngle()) + b->GetBonePos();
        FPoint o = *FPoint(0.f, b->GetLength() * 0.314f).Rotate(b->GetBoneAngle());

        points.push_back(p1 + o);
        points.push_back(p1 - o);
        points.push_back(p2 - o);
        points.push_back(p2 + o);

        _meshes[_meshGenerateBone]->UpdatePoints(points);
    }
}

void Animation::IKBoneMove(const FPoint &point)
{
    if (_boneChain.size() <= 1)
    {
        BoneMoveTo(point, false);
        return;
    }

    //ik - search positions
    UpdateChain(_chainPoints, point);

    //update bone positiobns
    for (int i = 0; i < _boneChain.size(); ++i)
    {
        FPoint a = _boneChain[i]->GetBonePos();//points[i];
        FPoint b = _chainPoints[i + 1];

        if (_boneChain[i]->GetParent())
        {
            Matrix rev;
            rev.MakeRevers(_boneChain[i]->GetParent()->GetMatrix());

            rev.Mul(b);
        }

        _boneChain[i]->SetBoneAngle(atan2(b.y - a.y, b.x - a.x) + _deltaAngle[i]);
    }
}

void Animation::ExcludeChild()
{
    std::set<BoneAnimated*> sel;
    for (uint i = 0; i < _bones->size(); ++i)
    {
        (*_bones)[i]->SetMoveByParent(false);
    }
    for (std::set<uint>::iterator i = _selected.begin(); i != _selected.end(); ++i)
    {
        sel.insert((*_bones)[*i]);
    }
    for (uint i = 0; i < _bones->size(); ++i)
    {
        BoneAnimated *parent = (*_bones)[i]->GetParent();
        if (parent)
        {
            bool isParentSelected = ( sel.find(parent) != sel.end() );
            while (!isParentSelected && parent->GetParent())
            {                
                parent = parent->GetParent();
                isParentSelected |= ( parent && sel.find(parent) != sel.end() );
            }
            (*_bones)[i]->SetMoveByParent(isParentSelected);
        }
    }
}

void Animation::Picking(int index, bool add)
{
    if (index == -1 && !add)
    {
        _selected.clear();
        return;
    }
    if (index == -1)
    {
        return;
    }
    if (!add)
    {
        for (std::set<uint>::iterator i = _selected.begin(); i != _selected.end(); ++i)
        {
            if (*i == index)
            {
                return;
            }
        }
        _selected.clear();
        _selected.insert(index);
        ExcludeChild();
        return;
    }
    std::set<uint>::iterator i = _selected.find(index);
    if (i != _selected.end())
    {
        _selected.erase(i);
        return;
    }
    _selected.insert(index);
    ExcludeChild();
}

void Animation::SelectByArea(const Rect &area)
{
    _selected.clear();
    for (uint i = 0; i < _bones->size(); ++i)
    {
        if ((*_bones)[i]->IfInside(area))
        {
            _selected.insert(i);
        }
    }
    ExcludeChild();
}

void Animation::LinkBones(int parent, int child)
{
    if (parent >= 0 && child >= 0)
    {
        if ((*_bones)[child]->GetParent() != (*_bones)[parent])
        {
            (*_bones)[child]->SetParent((*_bones)[parent]);
        }
        return;
    }
    if (child >= 0)
    {
        (*_bones)[child]->SetParent(NULL);
        return;
    }
    if (parent >= 0)
    {
        assert(false);
        // todo: i do not know what to do)
        return;
    }
}

void Animation::RemoveBones()
{
    for (std::set<uint>::iterator i = _selected.begin(); i != _selected.end(); ++i)
    {
        delete (*_bones)[*i];
        (*_bones)[*i] = NULL;
    }
    std::set<std::string> names;
    for (uint i = 0; i < _bones->size(); )
    {
        if ((*_bones)[i] == NULL)
        {
            (*_bones)[i] = _bones->back();
            _bones->pop_back();
        }
        else
        {
            names.insert((*_bones)[i]->GetName());
            ++i;
        }
    }

    if (_bones == &_bonesOrigin)
        _bones = &_bonesAnimation;
    else
        _bones = &_bonesOrigin;

    for (uint i = 0; i < _bones->size(); ++i)
    {
        names.insert((*_bones)[i]->GetName());
    }

    for (uint i = 0; i < _meshes.size();)
    {
        if (names.find(_meshes[i]->GetBone()) == names.end())
        {
            delete _meshes[i];
            _meshes[i] = _meshes.back();
            _meshes.pop_back();
        }
        else
            ++i;
    }
    _selected.clear();
}

void Animation::Remove()
{
    if (MainWindow::Instance()->CreateDotMode())
    {
        for (uint i = 0; i < _meshes.size(); ++i)
        {
            if (_meshes[i]->RemoveDots())
            {
                return;
            }
        }
    }
    RemoveBones();
}

void Animation::Unlink()
{
    for (std::set<uint>::iterator i = _selected.begin(); i != _selected.end(); ++i)
    {
        (*_bones)[*i]->SetParent(NULL);
    }
}

void UpdateChain(PointList &points, const FPoint &target)
{
    FPoint &b = points.back();
    int counter = 0;
    while ((b - target).Length() > 1e-1f && counter < 10)
    {
        for (int i = points.size() - 2; i >= 0; --i)
        {
            FPoint &a = points[i];

            FPoint d(b - a);
            FPoint t(target - a);

            float angle = atan2(t.y, t.x) - atan2(d.y, d.x);

            for (int j = i + 1; j < points.size(); ++j)
            {
                FPoint p(points[j] - a);
                p.Rotate(angle);
                points[j] = p + a;
            }

            if ((b - target).Length() < 1e-1f)
            {
                return;
            }
        }
        counter++;
    }
}

bool Animation::SetTexture(const char *fileName)
{
    if (_texture)
        delete _texture;
    if (_baseSprite)
        delete _baseSprite;
    _texture = new GLTexture2D(fileName);
    if (!_texture->failed())
    {
        _baseSprite = new Sprite(_texture, 0, 0, _texture->Width(), _texture->Height());
    }
    if (!_texture->failed())
    {
        _fileName = fileName;
        for (uint i = 0; i < _meshes.size(); ++i)
        {
            _meshes[i]->GenerateTriangles();
        }
        return true;
    }
    else
    {
        _fileName = "";
        return false;
    }
}

void Animation::StartBoneCreating(uint index, const FPoint &point)
{
    _boneMoving = false;
    _startMovingPos = point;
    _startRotateAngle = 0.f;
    _startMovingBone = index;
}

FPoint Animation::GetBoneEnd(uint index)
{
    FPoint r((*_bones)[index]->GetLength(), 0.f);
    (*_bones)[index]->GetMatrix().Mul(r);
    return r;
}

void Animation::Finish()
{
    _meshGenerateBone = -1;
}

bool Animation::PolygonActive(const FPoint &pos)
{
    if (MainWindow::Instance()->CreateDotMode() && _selected.size() == 1)
    {
        for (uint i = 0; i < _meshes.size(); ++i)
        {
            if (_meshes[i]->GetBone() == (*_bones)[*_selected.begin()]->GetName())
            {
                if (_meshes[i]->PixelCheck(pos))
                    return true;
            }
        }
    }
    return false;
}

void Animation::PolygonMouseDown(const FPoint &pos)
{
    if (MainWindow::Instance()->CreateDotMode() && _selected.size() == 1)
    {
        for (uint i = 0; i < _meshes.size(); ++i)
        {
            if (_meshes[i]->GetBone() == (*_bones)[*_selected.begin()]->GetName())
            {
                _meshes[i]->MouseDown(pos);
            }
        }
    }
}

void Animation::PolygonMouseMove(const FPoint &pos)
{
    if (MainWindow::Instance()->CreateDotMode() && _selected.size() == 1)
    {
        for (uint i = 0; i < _meshes.size(); ++i)
        {
            if (_meshes[i]->GetBone() == (*_bones)[*_selected.begin()]->GetName())
            {
                _meshes[i]->MouseMove(pos);
            }
        }
    }
}

void Animation::PolygonMouseUp(const FPoint &pos)
{
    if (MainWindow::Instance()->CreateDotMode() && _selected.size() == 1)
    {
        for (uint i = 0; i < _meshes.size(); ++i)
        {
            if (_meshes[i]->GetBone() == (*_bones)[*_selected.begin()]->GetName())
            {
                _meshes[i]->MouseUp(pos);
            }
        }
    }
}

void Animation::LoadFromFile(const std::string &fileName)
{
    _boneMoving = false;
    _startMovingBone = -1;

//    if (_texture)
//        delete _texture;

//    if (_baseSprite)
//        delete _baseSprite;

//    _texture = NULL;
//    _baseSprite = NULL;
    _meshGenerateBone = -1;
    _selected.clear();

    for (uint i = 0; i < _meshes.size(); ++i)
    {
        delete _meshes[i];
    }
    _meshes.clear();

    for (uint i = 0; i < _bonesOrigin.size(); ++i)
    {
        delete _bonesOrigin[i];
    }
    _bonesOrigin.clear();

    for (uint i = 0; i < _bonesAnimation.size(); ++i)
    {
        delete _bonesAnimation[i];
    }
    _bonesAnimation.clear();

    _deltaAngle.clear();
    _boneChain.clear();
    _chainPoints.clear();

    std::ifstream myfile(fileName.c_str());
    std::vector<char> buffer((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>( ));

    if (buffer.size())
    { //    -  XML
        buffer.push_back('\0');
        rapidxml::xml_document<> doc;
        doc.parse<0>(&buffer[0]);
        rapidxml::xml_node<> *root = doc.first_node();

        { // load meshes
            rapidxml::xml_node<> *meshes = root->first_node("meshes");
            rapidxml::xml_node<> *mesh = meshes->first_node("mesh");
            _meshes.resize(atoi(meshes->first_attribute("size")->value()));
            int counter = 0;
            while (mesh)
            {
                assert(counter < _meshes.size());
                _meshes[counter] = new ColoredPolygon(mesh);
                counter++;
                mesh = mesh->next_sibling("mesh");
            }
            assert(counter == _meshes.size());
        }
        { // load bones origin
            rapidxml::xml_node<> *bones = root->first_node("origin");
            rapidxml::xml_node<> *bone = bones ? bones->first_node("bone") : NULL;
            int n = bone ? atoi(bones->first_attribute("size")->value()) : 0;
            _bones = &_bonesOrigin;
            while (bone)
            {
                _bonesOrigin.push_back(new BoneAnimated(bone));
                bone = bone->next_sibling("bone");
            }
            assert(n == _bonesOrigin.size());
            _bones = NULL;
        }
        { // load animation
            rapidxml::xml_node<> *bones = root->first_node("animation");
            rapidxml::xml_node<> *bone = bones ? bones->first_node("bone") : NULL;
            int n = bone ? atoi(bones->first_attribute("size")->value()) : 0;
            _bones = &_bonesAnimation;
            while (bone)
            {
                _bonesAnimation.push_back(new BoneAnimated(bone));
                bone = bone->next_sibling("bone");
            }
            assert(n == _bonesAnimation.size());
            _bones = NULL;
        }
    }
}

void Animation::RegisterBone(BoneAnimated *b)
{
    _bones->push_back(b);
}

void Animation::SaveToFile(const std::string &fileName)
{
    std::ofstream file_stored(fileName.c_str());

    rapidxml::xml_document<> doc;
    rapidxml::xml_node<> *xe = doc.allocate_node(rapidxml::node_element, "root");
    doc.append_node(xe);

    // save bones
    rapidxml::xml_node<> *bones = xe->document()->allocate_node(rapidxml::node_element, "origin");
    xe->append_node(bones);

    Math::Write(bones, "size", (int)_bonesOrigin.size());
    for (BoneList::iterator i = _bonesOrigin.begin(), e = _bonesOrigin.end(); i != e; ++i)
    {
        if ((*i)->GetParent() == NULL)
        {
            rapidxml::xml_node<> *bone = xe->document()->allocate_node(rapidxml::node_element, "bone");
            bones->append_node(bone);
            (*i)->SaveToXml(bone);
        }
    }

    // save meshes
    rapidxml::xml_node<> *meshes = xe->document()->allocate_node(rapidxml::node_element, "meshes");
    xe->append_node(meshes);

    Math::Write(meshes, "size", (int)_meshes.size());
    for (uint i = 0; i < _meshes.size(); ++i)
    {
        rapidxml::xml_node<> *mesh = xe->document()->allocate_node(rapidxml::node_element, "mesh");
        meshes->append_node(mesh);
        _meshes[i]->SaveToXml(mesh);
    }

    file_stored << doc;
    file_stored.close();
}

void Animation::CreatePointMass()
{
    for (uint j = 0; j < _meshes.size(); ++j)
    {
        for (std::set<uint>::iterator i = _selected.begin(); i != _selected.end(); ++i)
        {
            if (_meshes[j]->GetBone() == (*_bones)[(*i)]->GetName())
            {
                _meshes[j]->BindToBone((*_bones)[(*i)]);
            }
        }
    }
}

void Animation::DuplicateNewBones()
{
    std::set<std::string> boneNames;
    for (uint i = 0; i < _bonesAnimation.size(); ++i)
    {
        boneNames.insert(_bonesAnimation[i]->GetName());
    }

    std::set<std::string> meshedNames;
    for (uint i = 0; i < _meshes.size(); ++i)
    {
        meshedNames.insert(_meshes[i]->GetBone());
    }

    BoneList nb;
    for (uint i = 0; i < _bonesOrigin.size(); ++i)
    {
        if (boneNames.find(_bonesOrigin[i]->GetName()) == boneNames.end()
                && meshedNames.find(_bonesOrigin[i]->GetName()) != meshedNames.end())
        {
            nb.push_back(_bonesOrigin[i]);

            for (uint j = 0; j < _meshes.size(); ++j)
            {
                if (_meshes[j]->GetBone() == _bonesOrigin[i]->GetName())
                {
                    _meshes[j]->BindToBone(_bonesOrigin[i]);
                }
            }
        }
    }

    if (nb.size() > 0)
    {
        _bones = &_bonesAnimation;
        for (uint i = 0; i < nb.size(); ++i)
        {
            assert(nb[i]->GetParent() == NULL);
            _bonesAnimation.push_back(new BoneAnimated(*nb[i]));
        }
    }
}

void Animation::ReplaceBonesWith()
{
    for (auto m : _meshes)
    {
        m->ReplaceBonesWith(_bonesAnimation);
    }
}
