#include "WidgetCreator.h"
#include "./resources/const.h"
#include "./resources/color.h"
#include "./resources/kb_btn_id.h"

EmptyLayout *WidgetCreator::getEmptyLayout()
{
    try
    {
        EmptyLayout *layout = new EmptyLayout(1);
        layout->setBackColor(COLOR_MAIN_BACK);
        layout->setWidth(_display.width());
        layout->setHeight(_display.height());
        return layout;
    }
    catch (const std::bad_alloc &e)
    {
        log_e("%s", e.what());
        esp_restart();
    }
}

Label *WidgetCreator::getDisplayDescription(uint16_t id, const char *text)
{
    try
    {
        Label *display_descr = new Label(1);
        display_descr->setText(text);
        display_descr->setWidth(_display.width());
        display_descr->setHeight(DISPLAY_DESCRIPT_HEIGHT);
        display_descr->setPos(0, 0);
        display_descr->setTextColor(TFT_WHITE);
        display_descr->setBackColor(TFT_BLACK);
        display_descr->setAlign(IWidget::ALIGN_CENTER);
        display_descr->setGravity(IWidget::GRAVITY_CENTER);

        return display_descr;
    }
    catch (const std::bad_alloc &e)
    {
        log_e("%s", e.what());
        esp_restart();
    }
}

Label *WidgetCreator::getItemLabel(const char *text, uint8_t font_id, uint8_t text_size)
{
    try
    {
        Label *item = new Label(1);
        item->setText(text);
        item->setFontID(font_id);
        item->setTextSize(text_size);
        item->setGravity(IWidget::GRAVITY_CENTER);
        item->setTextOffset(3);
        item->setTickerInFocus(true);
        return item;
    }
    catch (const std::bad_alloc &e)
    {
        log_e("%s", e.what());
        esp_restart();
    }
}

MenuItem *WidgetCreator::getMenuItem(uint16_t id)
{
    try
    {
        MenuItem *item = new MenuItem(id);
        item->setFocusBorderColor(COLOR_LIME);
        item->setFocusBackColor(COLOR_FOCUS_BACK);
        item->setBackColor(COLOR_MENU_ITEM);
        item->setChangingBorder(true);
        item->setChangingBack(true);
        return item;
    }
    catch (const std::bad_alloc &e)
    {
        log_e("%s", e.what());
        esp_restart();
    }
}

DynamicMenu *WidgetCreator::getDynamicMenu(uint16_t id)
{
    try
    {
        DynamicMenu *menu = new DynamicMenu(id);
        menu->setBackColor(TFT_BLACK);
        menu->setWidth(_display.width());
        menu->setHeight(_display.height() - TFT_CUTOUT * 2 - 2);
        return menu;
    }
    catch (const std::bad_alloc &e)
    {
        log_e("%s", e.what());
        esp_restart();
    }
}

Label *WidgetCreator::getStatusMsgLable(uint16_t id, const char *text, uint8_t text_size)
{
    Label *lbl = new Label(id);
    lbl->setText(text);
    lbl->setTextSize(text_size);
    lbl->setAlign(IWidget::ALIGN_CENTER);
    lbl->setGravity(IWidget::GRAVITY_CENTER);
    lbl->setBackColor(COLOR_MAIN_BACK);
    lbl->setWidth(_display.width());
    lbl->setHeight(_display.height() - TFT_CUTOUT);

    return lbl;
}

Keyboard *WidgetCreator::getStandardEnKeyboard(uint16_t id)
{
    Keyboard *_keyboard = new Keyboard(id);
    _keyboard->setWidth(_display.width());
    _keyboard->setHeight(_display.height() - 50 - TFT_CUTOUT * 2);
    _keyboard->setPos(0, 50 + TFT_CUTOUT);
    _keyboard->setCornerRadius(5);

    KeyboardRow *row1 = new KeyboardRow(1); // Цифри
    _keyboard->addWidget(row1);
    row1->setHeight(25);
    row1->setBtnHeight(23);
    row1->setBtnWidth(18);
    //
    KeyboardRow *row2 = row1->clone(2);
    _keyboard->addWidget(row2);
    //
    KeyboardRow *row3 = row1->clone(3);
    _keyboard->addWidget(row3);
    row3->setBtnWidth(20);
    //
    KeyboardRow *row4 = row1->clone(4);
    _keyboard->addWidget(row4);
    row4->setBtnWidth(25);
    //
    KeyboardRow *row5 = row1->clone(5); // Символи
    _keyboard->addWidget(row5);
    row5->setBtnWidth(40);

    Label *q = new Label(ID_KBTN_Q);
    q->setText("q");
    q->setFontID(2);
    q->setGravity(IWidget::GRAVITY_CENTER);
    q->setAlign(IWidget::ALIGN_CENTER);
    q->setFocusBorderColor(COLOR_LIME);
    q->setChangingBorder(true);
    q->setFocusBackColor(COLOR_FOCUS_BACK);
    q->setChangingBack(true);
    q->setTextColor(TFT_WHITE);
    q->setBackColor(COLOR_MAIN_BACK);
    q->setCornerRadius(2);
    //
    Label *w = q->clone(ID_KBTN_W);
    w->setText("w");

    Label *e = q->clone(ID_KBTN_E);
    e->setText("e");

    Label *r = q->clone(ID_KBTN_R);
    r->setText("r");

    Label *t = q->clone(ID_KBTN_T);
    t->setText("t");

    Label *y = q->clone(ID_KBTN_Y);
    y->setText("y");

    Label *u = q->clone(ID_KBTN_U);
    u->setText("u");

    Label *i = q->clone(ID_KBTN_I);
    i->setText("i");

    Label *o = q->clone(ID_KBTN_O);
    o->setText("o");

    Label *p = q->clone(ID_KBTN_P);
    p->setText("p");

    Label *a = q->clone(ID_KBTN_A);
    a->setText("a");

    Label *s = q->clone(ID_KBTN_S);
    s->setText("s");

    Label *d = q->clone(ID_KBTN_D);
    d->setText("d");

    Label *f = q->clone(ID_KBTN_F);
    f->setText("f");

    Label *g = q->clone(ID_KBTN_G);
    g->setText("g");

    Label *h = q->clone(ID_KBTN_H);
    h->setText("h");

    Label *j = q->clone(ID_KBTN_J);
    j->setText("j");

    Label *k = q->clone(ID_KBTN_K);
    k->setText("k");

    Label *l = q->clone(ID_KBTN_L);
    l->setText("l");

    Label *z = q->clone(ID_KBTN_Z);
    z->setText("z");

    Label *x = q->clone(ID_KBTN_X);
    x->setText("x");

    Label *c = q->clone(ID_KBTN_C);
    c->setText("c");

    Label *v = q->clone(ID_KBTN_V);
    v->setText("v");

    Label *b = q->clone(ID_KBTN_B);
    b->setText("b");

    Label *n = q->clone(ID_KBTN_N);
    n->setText("n");

    Label *m = q->clone(ID_KBTN_M);
    m->setText("m");

    // -----

    Label *l0 = q->clone(ID_KBTN_0);
    l0->setText("0");

    Label *l1 = q->clone(ID_KBTN_1);
    l1->setText("1");

    Label *l2 = q->clone(ID_KBTN_2);
    l2->setText("2");

    Label *l3 = q->clone(ID_KBTN_3);
    l3->setText("3");

    Label *l4 = q->clone(ID_KBTN_4);
    l4->setText("4");

    Label *l5 = q->clone(ID_KBTN_5);
    l5->setText("5");

    Label *l6 = q->clone(ID_KBTN_6);
    l6->setText("6");

    Label *l7 = q->clone(ID_KBTN_7);
    l7->setText("7");

    Label *l8 = q->clone(ID_KBTN_8);
    l8->setText("8");

    Label *l9 = q->clone(ID_KBTN_9);
    l9->setText("9");

    //

    Label *l_dot = q->clone(ID_KBTN_DOT);
    l_dot->setText(".");

    Label *l_under = q->clone(ID_KBTN_UNDERLINE);
    l_under->setText("_");

    Label *l_hyp = q->clone(ID_KBTN_HYPHEN);
    l_hyp->setText("-");

    //
    row1->addWidget(l0);
    row1->addWidget(l1);
    row1->addWidget(l2);
    row1->addWidget(l3);
    row1->addWidget(l4);
    row1->addWidget(l5);
    row1->addWidget(l6);
    row1->addWidget(l7);
    row1->addWidget(l8);
    row1->addWidget(l9);
    //
    row2->addWidget(q);
    row2->addWidget(w);
    row2->addWidget(e);
    row2->addWidget(r);
    row2->addWidget(t);
    row2->addWidget(y);
    row2->addWidget(u);
    row2->addWidget(i);
    row2->addWidget(o);
    row2->addWidget(p);
    //
    row3->addWidget(a);
    row3->addWidget(s);
    row3->addWidget(d);
    row3->addWidget(f);
    row3->addWidget(g);
    row3->addWidget(h);
    row3->addWidget(j);
    row3->addWidget(k);
    row3->addWidget(l);
    //
    row4->addWidget(z);
    row4->addWidget(x);
    row4->addWidget(c);
    row4->addWidget(v);
    row4->addWidget(b);
    row4->addWidget(n);
    row4->addWidget(m);
    //
    row5->addWidget(l_dot);
    row5->addWidget(l_under);
    row5->addWidget(l_hyp);

    return _keyboard;
}
