#ifdef USE_GUI
#include "gui.h"
#include "raylib.h"
#include <cstring>
#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#ifdef _WIN32
extern void setAppIcon(void* hwnd);
#endif

// White bg; maroon info-boxes; dark text on white; white text on maroon
static const Color W_BG    = {255, 255, 255, 255}; // white screen bg
static const Color W_FORM  = {252, 248, 249, 255}; // near-white form panel bg
static const Color W_BOX   = {112,  15,  26, 255}; // maroon info box
static const Color W_BOX2  = {138,  24,  40, 255}; // lighter maroon sub-box
static const Color W_HDR   = { 65,   4,  13, 255}; // dark maroon header
static const Color W_BDR   = {175,  40,  56, 255}; // maroon border (on boxes)
static const Color W_FBDR  = {210, 195, 198, 255}; // soft border (on white bg)
static const Color W_TXT   = { 28,  20,  22, 255}; // dark text on white
static const Color W_MUT   = {130, 112, 116, 255}; // muted label on white
static const Color W_PTXT  = {255, 255, 255, 255}; // white text on maroon
static const Color W_PLBL  = {255, 200, 210, 255}; // light-pink label on maroon
static const Color W_ACN   = {112,  15,  26, 255}; // maroon (buttons on white)
static const Color W_SEC   = {220, 210, 212, 255}; // light-grey secondary btn
static const Color W_FIELD = {248, 243, 245, 255}; // input field bg
static const Color W_FDIM  = {158, 135, 140, 255}; // placeholder text
static const Color W_OK    = { 35, 168,  72, 255}; // green
static const Color W_ERR   = {210,  45,  45, 255}; // red
static const Color W_CAMP  = { 28, 155,  70, 255}; // campus green (selected)
static const Color W_LCMP  = {185, 230, 200, 255}; // campus light (unselected)
static const Color W_LNRM  = {232, 220, 222, 255}; // normal area (unselected)

static const int W = 1920, H = 1080;

// Font wrappers — all text goes through ABeeZee
static Font g_font;
static const float g_sp     = 1.0f;
static const float g_fscale = 1.35f; // global font size multiplier

static void dtxt(const char* text, int x, int y, int size, Color color) {
    DrawTextEx(g_font, text, {(float)x, (float)y}, size * g_fscale, g_sp, color);
}
static int mtxt(const char* text, int size) {
    return (int)MeasureTextEx(g_font, text, size * g_fscale, g_sp).x;
}

enum class Screen {
    ROLE_SELECT, LOGIN, REGISTER_FORM,
    DRIVER_MENU, CREATE_RIDE, CURRENT_RIDE, RIDE_HISTORY,
    PASSENGER_MENU, SEARCH_RIDE, SEARCH_RESULTS, EDIT_PROFILE
};

struct TIn {
    char buf[64] = {};
    bool on = false;
    void clear() { buf[0] = '\0'; on = false; }
    std::string str() const { return buf; }
};

// ---- widget helpers ----

static bool btn(Rectangle r, const char* lbl, Color bg, Color fg = WHITE, int fs = 18) {
    bool ov = CheckCollisionPointRec(GetMousePosition(), r);
    Color c = ov ? Color{(unsigned char)std::max(0, bg.r-22),
                         (unsigned char)std::max(0, bg.g-22),
                         (unsigned char)std::max(0, bg.b-22), 255} : bg;
    DrawRectangleRec(r, c);
    DrawRectangleLinesEx(r, 1.0f, W_FBDR);
    int tw = mtxt(lbl, fs);
    dtxt(lbl, (int)(r.x+(r.width-tw)/2), (int)(r.y+(r.height-fs)/2), fs, fg);
    return ov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
static bool btnP(Rectangle r, const char* lbl, int fs = 18) { return btn(r, lbl, W_ACN, WHITE,  fs); }
static bool btnD(Rectangle r, const char* lbl, int fs = 18) { return btn(r, lbl, W_ERR, WHITE,  fs); }
static bool btnS(Rectangle r, const char* lbl, int fs = 16) { return btn(r, lbl, W_SEC, W_TXT,  fs); }

// maroon info box
static void mbox(float x, float y, float w, float h) {
    DrawRectangle((int)x,(int)y,(int)w,(int)h, W_BOX);
    DrawRectangleLinesEx({x,y,w,h}, 1.5f, W_BDR);
}
// white form card
static void fbox(float x, float y, float w, float h) {
    DrawRectangle((int)x,(int)y,(int)w,(int)h, W_FORM);
    DrawRectangleLinesEx({x,y,w,h}, 1.5f, W_FBDR);
}

static void hdr(const char* title) {
    DrawRectangle(0, 0, W, 68, W_HDR);
    DrawRectangle(0, 68, W, 2, W_BDR);
    dtxt("IBA Carpool", 22, 22, 22, WHITE);
    int tw = mtxt(title, 20);
    dtxt(title, (W-tw)/2, 24, 20, WHITE);
}

static void field(TIn& t, Rectangle r, const char* ph, bool numOnly = false, int maxLen = 62) {
    bool ov = CheckCollisionPointRec(GetMousePosition(), r);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) t.on = ov;
    DrawRectangleRec(r, W_FIELD);
    DrawRectangleLinesEx(r, 1.5f, t.on ? W_ACN : W_FBDR);
    if (t.on) {
        int c;
        while ((c = GetCharPressed()) > 0) {
            int len = (int)strlen(t.buf);
            bool ok = numOnly ? (c>='0'&&c<='9') : (c>=32&&c<=126);
            if (len < maxLen && ok) { t.buf[len]=(char)c; t.buf[len+1]='\0'; }
        }
        if (IsKeyPressed(KEY_BACKSPACE)) { int l=(int)strlen(t.buf); if(l)t.buf[l-1]='\0'; }
    }
    dtxt(t.buf[0]?t.buf:ph, (int)r.x+10, (int)(r.y+(r.height-18)/2), 18,
             t.buf[0] ? W_TXT : W_FDIM);
    if (t.on && (int)(GetTime()*2)%2==0) {
        int cx=(int)r.x+10+mtxt(t.buf,18)+2;
        DrawRectangle(cx,(int)(r.y+(r.height-20)/2),2,20,W_TXT);
    }
}

// 3-column area picker — light bg for unselected, maroon/green for selected
static int areaPick(float x, float y, float gw, int sel, const char* lbl) {
    dtxt(lbl,(int)x,(int)y-22,16,W_MUT);
    float bw=(gw-14)/3.0f, bh=34;
    int result=sel;
    for (int i=0;i<AREA_COUNT;++i) {
        int row=i/3, col=i%3;
        Rectangle r={x+col*(bw+7), y+row*(bh+7), bw, bh};
        bool ov=CheckCollisionPointRec(GetMousePosition(),r);
        bool isc=isIBACampus(intToArea(i));
        Color bg = (i==sel) ? (isc?W_CAMP:W_ACN)
                 :  ov      ? Color{205,192,195,255}
                 :  isc     ? W_LCMP
                            : W_LNRM;
        DrawRectangleRec(r,bg);
        DrawRectangleLinesEx(r,1.0f,W_FBDR);
        std::string nm=areaToStr(intToArea(i));
        if (nm.size()>14) nm=nm.substr(0,13)+".";
        int ts=12, tw=mtxt(nm.c_str(),ts);
        // dark text on light bg; white on maroon/green selected
        Color tc = (i==sel) ? WHITE : (isc ? Color{20,100,45,255} : W_TXT);
        dtxt(nm.c_str(),(int)(r.x+(r.width-tw)/2),(int)(r.y+(r.height-ts)/2),ts,tc);
        if (ov&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) result=i;
    }
    return result;
}

// X exit button (top-right)
static void xbtn(bool& exitFlag) {
    Rectangle r={(float)(W-50),12,38,44};
    bool ov=CheckCollisionPointRec(GetMousePosition(),r);
    DrawRectangleRec(r, ov?Color{240,60,60,255}:W_ERR);
    int cx=(int)r.x+19, cy=(int)r.y+22;
    DrawLineEx({(float)(cx-8),(float)(cy-8)},{(float)(cx+8),(float)(cy+8)},2.5f,WHITE);
    DrawLineEx({(float)(cx+8),(float)(cy-8)},{(float)(cx-8),(float)(cy+8)},2.5f,WHITE);
    if (ov&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) exitFlag=true;
}

// Back button placed well clear of X (ends at W-70, 20px gap before X at W-50)
// width=160, so x = W-70-160 = W-230
static bool backBtn() {
    return btnS({(float)(W-230), 16, 160, 40}, "<- Back");
}

// ---- GUI class ----

class GUI3 {
    CarpoolSystem& sys;
    Screen screen = Screen::ROLE_SELECT;
    bool   drv    = false;
    bool   quit   = false;
    std::string uid, err;

    TIn reg_name, reg_phone, reg_vmake, reg_vmodel, reg_vplate, reg_vcap;
    int reg_gender=0, reg_vtype=0;
    std::string reg_ok;

    int cr_pu=0, cr_do=11, cr_pref=2, cr_doff=1;
    TIn cr_time;

    int sr_pu=0, sr_do=11;
    TIn sr_time;
    std::vector<std::string> sr_res;

    TIn ep_name, ep_phone, ep_vmake, ep_vmodel, ep_vplate, ep_vcap;
    int  ep_gender=0;
    bool ep_del=false;
    std::string ep_ok;

    void go(Screen s) { err=""; screen=s; sys.checkAutoComplete(); }

    // ---- screens ----

    void drawRoleSelect() {
        ClearBackground(W_BG);
        const char* t="IBA Carpool System";
        dtxt(t,(W-mtxt(t,42))/2,190,42,W_TXT);
        const char* s="IBA-specific carpooling";
        dtxt(s,(W-mtxt(s,18))/2,248,18,W_MUT);

        float bw=310,bh=62,gap=18,sx=(W-2*bw-gap)/2.0f,sy=360;
        if (btnP({sx,        sy,      bw,bh},"Driver - Login"))      { drv=true;  go(Screen::LOGIN); }
        if (btnP({sx+bw+gap, sy,      bw,bh},"Passenger - Login"))   { drv=false; go(Screen::LOGIN); }
        if (btn ({sx,        sy+bh+gap,bw,bh},"Driver - Register",   W_SEC,W_TXT)) {
            drv=true; reg_name.clear(); reg_phone.clear(); reg_vmake.clear();
            reg_vmodel.clear(); reg_vplate.clear(); reg_vcap.clear();
            reg_gender=0; reg_vtype=0; reg_ok=""; go(Screen::REGISTER_FORM);
        }
        if (btn ({sx+bw+gap, sy+bh+gap,bw,bh},"Passenger - Register",W_SEC,W_TXT)) {
            drv=false; reg_name.clear(); reg_phone.clear();
            reg_gender=0; reg_vtype=0; reg_ok=""; go(Screen::REGISTER_FORM);
        }
    }

    void drawLogin() {
        ClearBackground(W_BG);
        hdr(drv?"Driver Login":"Passenger Login");
        float pw=460,ph=270;
        Rectangle p={(W-pw)/2,(H-ph)/2-20,pw,ph};
        fbox(p.x,p.y,pw,ph);
        dtxt("Student ID",(int)(p.x+22),(int)(p.y+24),18,W_TXT);
        static TIn lid;
        field(lid,{p.x+22,p.y+54,pw-44,46},"e.g. STU-1");
        if (!err.empty()) dtxt(err.c_str(),(int)(p.x+22),(int)(p.y+114),15,W_ERR);
        if (btnP({p.x+22,p.y+148,pw-44,46},"Login")) {
            auto* st=sys.getStudent(lid.str());
            if (!st)                          err="ID not found.";
            else if (drv&&!st->getIsDriver()) err="Not a driver account.";
            else if (!drv&&st->getIsDriver()) err="Not a passenger account.";
            else { uid=lid.str(); lid.clear(); go(drv?Screen::DRIVER_MENU:Screen::PASSENGER_MENU); }
        }
        if (btnS({p.x+22,      p.y+208,120,38},"<- Back"))   go(Screen::ROLE_SELECT);
        if (btnS({p.x+pw-142,  p.y+208,120,38},"Register")) {
            reg_name.clear(); reg_phone.clear(); reg_vmake.clear();
            reg_vmodel.clear(); reg_vplate.clear(); reg_vcap.clear();
            reg_gender=0; reg_vtype=0; reg_ok=""; go(Screen::REGISTER_FORM);
        }
    }

    void drawRegister() {
        ClearBackground(W_BG);
        hdr(drv?"Register Driver":"Register Passenger");
        float x=50,y=80,iw=400,ih=46;
        auto lbl=[&](const char* s){ dtxt(s,(int)x,(int)y,15,W_MUT); y+=20; };

        lbl("Full Name");
        field(reg_name,{x,y,iw,ih},"e.g. Ali Khan"); y+=ih+12;
        lbl("Phone (digits, max 11)");
        field(reg_phone,{x,y,iw,ih},"03001234567",true,11); y+=ih+12;

        dtxt("Gender",(int)x,(int)y,15,W_MUT); y+=20;
        if (btn({x,    y,194,40},"Male",   reg_gender==0?W_ACN:W_SEC, reg_gender==0?WHITE:W_TXT)) reg_gender=0;
        if (btn({x+200,y,194,40},"Female", reg_gender==1?W_ACN:W_SEC, reg_gender==1?WHITE:W_TXT)) reg_gender=1;
        y+=52;

        if (drv) {
            dtxt("Vehicle Type",(int)x,(int)y,15,W_MUT); y+=20;
            if (btn({x,    y,194,40},"Car",  reg_vtype==0?W_ACN:W_SEC, reg_vtype==0?WHITE:W_TXT)) reg_vtype=0;
            if (btn({x+200,y,194,40},"Bike", reg_vtype==1?W_ACN:W_SEC, reg_vtype==1?WHITE:W_TXT)) reg_vtype=1;
            y+=52;
            lbl("Make");  field(reg_vmake, {x,y,iw,ih},"Toyota");   y+=ih+12;
            lbl("Model"); field(reg_vmodel,{x,y,iw,ih},"Corolla");  y+=ih+12;
            lbl("Plate"); field(reg_vplate,{x,y,iw,ih},"KHI-0000"); y+=ih+12;
            if (reg_vtype==0){ lbl("Seats (1-6)"); field(reg_vcap,{x,y,iw,ih},"4"); y+=ih+12; }
            else { dtxt("Bike: 1 seat (fixed)",(int)x,(int)y,14,W_MUT); y+=28; }
        }

        if (!reg_ok.empty()) { dtxt(reg_ok.c_str(),(int)x,(int)y,16,W_OK);  y+=28; }
        if (!err.empty())    { dtxt(err.c_str(),   (int)x,(int)y,16,W_ERR); y+=28; }

        if (btnP({x,y,210,48},"Create Account")) {
            try {
                std::string nm=reg_name.str(), ph=reg_phone.str();
                if (nm.empty()) throw std::runtime_error("Name required.");
                if (ph.empty()) throw std::runtime_error("Phone required.");
                if ((int)ph.size()>11) throw std::runtime_error("Phone max 11 digits.");
                for (char c:ph) if(!isdigit((unsigned char)c)) throw std::runtime_error("Digits only.");
                if (drv&&(reg_vmake.str().empty()||reg_vmodel.str().empty()||reg_vplate.str().empty()))
                    throw std::runtime_error("All vehicle fields required.");
                Gender g=reg_gender==0?Gender::MALE:Gender::FEMALE;
                std::string sid=sys.createStudent(nm,ph,g,drv);
                if (drv) {
                    int cap=1;
                    if (reg_vtype==0) {
                        try { cap=reg_vcap.str().empty()?3:std::stoi(reg_vcap.str()); }
                        catch(...){ throw std::runtime_error("Seats must be a number."); }
                        if (cap<1||cap>6) throw std::runtime_error("Seats 1-6.");
                        sys.addCar(sid,reg_vmake.str(),reg_vmodel.str(),reg_vplate.str(),cap);
                    } else sys.addBike(sid,reg_vmake.str(),reg_vmodel.str(),reg_vplate.str(),1);
                }
                reg_ok="Account created! ID: "+sid; err="";
                reg_name.clear(); reg_phone.clear();
                reg_vmake.clear(); reg_vmodel.clear(); reg_vplate.clear(); reg_vcap.clear();
            } catch(std::runtime_error& e){ err=e.what(); reg_ok=""; }
        }
        if (backBtn()) go(Screen::LOGIN);
    }

    void drawDriverMenu() {
        ClearBackground(W_BG);
        auto* st=sys.getStudent(uid);
        if (!st){ go(Screen::ROLE_SELECT); return; }
        hdr("Driver Dashboard");

        dtxt(("Welcome, "+st->getName()).c_str(),50,88,26,W_TXT);
        dtxt(("Rides completed: "+std::to_string(st->getRidesCompleted())).c_str(),50,124,17,W_MUT);

        std::string aid=sys.getDriverActiveRide(uid);
        float x=50,y=172,bw=360,bh=52,gap=12;

        if (aid.empty()) {
            if (btnP({x,y,bw,bh},"+ Create Ride")) {
                cr_pu=0; cr_do=11; cr_pref=2; cr_time.clear(); cr_doff=1;
                go(Screen::CREATE_RIDE);
            }
        } else {
            if (btn({x,y,bw,bh},("Current Ride: "+aid).c_str(),W_BOX,WHITE))
                go(Screen::CURRENT_RIDE);
        }
        y+=bh+gap;
        if (btn({x,y,bw,bh},"Ride History", W_BOX,WHITE)) go(Screen::RIDE_HISTORY);
        y+=bh+gap;
        if (btn({x,y,bw,bh},"Edit Profile", W_BOX,WHITE)) {
            auto* s=sys.getStudent(uid);
            if (s) {
                strncpy(ep_name.buf,  s->getName().c_str(),        62);
                strncpy(ep_phone.buf, s->getPhoneNumber().c_str(), 62);
                ep_gender=(s->getGender()==Gender::MALE)?0:1;
                ep_del=false; ep_ok=""; ep_name.on=ep_phone.on=false;
                auto* v=sys.getVehicle(sys.getVehicleForDriver(uid));
                if (v) {
                    strncpy(ep_vmake.buf,  v->getMake().c_str(),         62);
                    strncpy(ep_vmodel.buf, v->getModel().c_str(),        62);
                    strncpy(ep_vplate.buf, v->getLicensePlate().c_str(), 62);
                    snprintf(ep_vcap.buf,62,"%d",v->getCapacity());
                    ep_vmake.on=ep_vmodel.on=ep_vplate.on=ep_vcap.on=false;
                }
            }
            go(Screen::EDIT_PROFILE);
        }
        y+=bh+gap+16;
        if (btnD({x,y,bw,bh},"Logout")) { uid=""; go(Screen::ROLE_SELECT); }
    }

    void drawCreateRide() {
        ClearBackground(W_BG);
        hdr("Create Ride");
        float lx=50,rx=W/2.0f+30,top=74;
        cr_pu=areaPick(lx,top+28,W/2.0f-80,cr_pu,"Pickup Area");
        cr_do=areaPick(rx,top+28,W/2.0f-80,cr_do,"Drop-off Area");

        int rows=(AREA_COUNT+2)/3;
        float by=(top+28)+(rows-1)*41.0f+34.0f+26;

        dtxt("Time (HH:MM)",(int)lx,(int)by,16,W_MUT); by+=20;
        field(cr_time,{lx,by,170,44},"08:30");

        dtxt("Date",(int)(lx+196),(int)(by-20),16,W_MUT);
        const char* dl[]={"Today","Tomorrow","+2 Days"};
        for (int i=0;i<3;++i)
            if (btn({lx+196+(float)i*114,by,106,44},dl[i],
                    cr_doff==i?W_ACN:W_SEC, cr_doff==i?WHITE:W_TXT,14)) cr_doff=i;
        by+=54;

        dtxt("Gender Pref",(int)lx,(int)by,16,W_MUT); by+=20;
        const char* pl[]={"Boys Only","Girls Only","Hybrid"};
        for (int i=0;i<3;++i)
            if (btn({lx+(float)i*154,by,146,42},pl[i],
                    cr_pref==i?W_ACN:W_SEC, cr_pref==i?WHITE:W_TXT,14)) cr_pref=i;
        by+=54;

        auto* veh=sys.getVehicle(sys.getVehicleForDriver(uid));
        if (veh) dtxt((veh->getType()+": "+veh->getMake()+" "+veh->getModel()).c_str(),(int)lx,(int)by,15,W_MUT);
        else     dtxt("No vehicle on account.",(int)lx,(int)by,15,W_ERR);
        by+=28;

        if (!err.empty()) { dtxt(err.c_str(),(int)lx,(int)by,15,W_ERR); by+=24; }

        std::string vehId=sys.getVehicleForDriver(uid);
        if (veh && btnP({lx,by,200,48},"Create Ride")) {
            std::string t=cr_time.str();
            int th=-1,tm=-1;
            if (t.size()==5&&t[2]==':')
                try { th=std::stoi(t.substr(0,2)); tm=std::stoi(t.substr(3,2)); } catch(...){}
            if (th<0||th>23||tm<0||tm>59)   err="Invalid time (HH:MM).";
            else if (cr_pu==cr_do)           err="Pickup and drop-off must differ.";
            else if (!isIBACampus(intToArea(cr_pu))&&!isIBACampus(intToArea(cr_do)))
                                             err="One area must be an IBA campus.";
            else {
                std::string d=dateOffset(cr_doff);
                if (isDateTimePast(d,t))     err="Time must be in the future.";
                else {
                    GenderPref gp=cr_pref==0?GenderPref::BOYS_ONLY:
                                  cr_pref==1?GenderPref::GIRLS_ONLY:GenderPref::HYBRID;
                    std::string rid=sys.createRide(uid,vehId,d,t,intToArea(cr_pu),intToArea(cr_do),gp);
                    if (rid.empty()) err="Could not create ride.";
                    else go(Screen::DRIVER_MENU);
                }
            }
        }
        if (backBtn()) go(Screen::DRIVER_MENU);
    }

    void drawCurrentRide() {
        ClearBackground(W_BG);
        hdr("Current Ride");
        std::string aid=sys.getDriverActiveRide(uid);
        auto* r=aid.empty()?nullptr:sys.getRide(aid);
        if (!r){ go(Screen::DRIVER_MENU); return; }

        float x=50,y=86;
        mbox(x,y,500,236);
        dtxt(aid.c_str(),(int)(x+16),(int)(y+14),20,WHITE);
        y+=46;
        auto row=[&](const char* k,const std::string& v){
            dtxt(k,         (int)(x+16),(int)y,14,W_PLBL);
            dtxt(v.c_str(), (int)(x+148),(int)y,14,WHITE);
            y+=22;
        };
        row("Pickup:",    areaToStr(r->getPickupArea()));
        row("Drop-off:",  areaToStr(r->getDropOffArea()));
        row("Date:",      r->getRideDate());
        row("Time:",      r->getRideTime());
        row("Pref:",      genderPrefToStr(r->getGenderPref()));
        row("Status:",    rideStatusToStr(r->getStatus()));
        row("Seats left:",std::to_string(r->getSeatsLeft()));

        y+=10;
        if (!isDateTimePast(r->getRideDate(),r->getRideTime()))
            if (btnD({x,y,180,42},"Cancel Ride",15))
                { sys.cancelRide(aid); go(Screen::DRIVER_MENU); }

        float px=x+540;
        dtxt("Passengers",(int)px,90,20,W_TXT);
        float py=122;
        auto& pax=r->getPassengers();
        if (pax.empty()) dtxt("None yet.",(int)px,(int)py,16,W_MUT);
        for (auto& pid:pax){
            auto* ps=sys.getStudent(pid);
            if (!ps) continue;
            mbox(px,py,440,56);
            dtxt(ps->getName().c_str(),        (int)(px+12),(int)(py+8), 17,WHITE);
            dtxt(ps->getPhoneNumber().c_str(), (int)(px+12),(int)(py+32),13,W_PLBL);
            py+=66;
        }
        if (backBtn()) go(Screen::DRIVER_MENU);
    }

    void drawHistory() {
        ClearBackground(W_BG);
        hdr("Ride History");
        auto hist=sys.getDriverHistory(uid);
        float x=50,y=86,rw=W-100;
        if (hist.empty()) dtxt("No history yet.",(int)x,(int)y,18,W_MUT);
        int n=0;
        for (auto& id:hist){
            if (n>=8) break;
            auto* r=sys.getRide(id);
            if (!r) continue;
            bool can=(r->getStatus()==RideStatus::CANCELED);
            mbox(x,y,rw,88);
            dtxt(id.c_str(),(int)(x+16),(int)(y+10),17,can?Color{255,130,130,255}:Color{140,240,170,255});
            dtxt((areaToStr(r->getPickupArea())+" -> "+areaToStr(r->getDropOffArea())).c_str(),
                     (int)(x+16),(int)(y+36),15,WHITE);
            dtxt((r->getRideDate()+"  "+r->getRideTime()+"  "+rideStatusToStr(r->getStatus())).c_str(),
                     (int)(x+16),(int)(y+62),13,W_PLBL);
            y+=98; n++;
        }
        if ((int)hist.size()>8) dtxt("(showing 8 most recent)",(int)x,(int)y,13,W_MUT);
        if (backBtn()) go(Screen::DRIVER_MENU);
    }

    void drawPassengerMenu() {
        ClearBackground(W_BG);
        auto* st=sys.getStudent(uid);
        if (!st){ go(Screen::ROLE_SELECT); return; }
        hdr("Passenger Dashboard");

        dtxt(("Welcome, "+st->getName()).c_str(),50,88,26,W_TXT);
        dtxt(("Rides completed: "+std::to_string(st->getRidesCompleted())).c_str(),50,124,17,W_MUT);

        float x=50,y=172,bw=360,bh=52,gap=12;
        if (btnP({x,y,bw,bh},"Search Rides"))
            { sr_pu=0; sr_do=11; sr_time.clear(); sr_res.clear(); go(Screen::SEARCH_RIDE); }
        y+=bh+gap;
        if (btn({x,y,bw,bh},"Edit Profile",W_BOX,WHITE)) {
            auto* s=sys.getStudent(uid);
            if (s){
                strncpy(ep_name.buf,  s->getName().c_str(),        62);
                strncpy(ep_phone.buf, s->getPhoneNumber().c_str(), 62);
                ep_gender=(s->getGender()==Gender::MALE)?0:1;
                ep_del=false; ep_ok=""; ep_name.on=ep_phone.on=false;
            }
            go(Screen::EDIT_PROFILE);
        }
        y+=bh+gap+16;
        if (btnD({x,y,bw,bh},"Logout")) { uid=""; go(Screen::ROLE_SELECT); }

        float nx=x+bw+50,ny=172,nw=W-nx-50;
        mbox(nx,ny,nw,H-ny-30);
        dtxt("Notifications",(int)(nx+16),(int)(ny+14),18,Color{255,220,100,255});
        auto& notifs=sys.getNotifications(uid);
        if (notifs.empty()) dtxt("None.",(int)(nx+16),(int)(ny+50),15,W_PLBL);
        float py=ny+50; int n=0;
        for (auto& notif:notifs){
            if (n>=10) break;
            DrawRectangle((int)(nx+8),(int)py,(int)(nw-16),56,W_BOX2);
            DrawRectangleLinesEx({nx+8,py,nw-16,56},1.0f,W_BDR);
            std::string msg=notif.getMessage();
            if (msg.size()>54){
                dtxt(msg.substr(0,54).c_str(),(int)(nx+16),(int)(py+4), 12,WHITE);
                dtxt(msg.substr(54).c_str(),  (int)(nx+16),(int)(py+22),12,WHITE);
            } else dtxt(msg.c_str(),(int)(nx+16),(int)(py+18),12,WHITE);
            py+=64; n++;
        }
    }

    void drawSearchRide() {
        ClearBackground(W_BG);
        hdr("Search Rides");
        float lx=50,rx=W/2.0f+30,top=74;
        sr_pu=areaPick(lx,top+28,W/2.0f-80,sr_pu,"Pickup Area");
        sr_do=areaPick(rx,top+28,W/2.0f-80,sr_do,"Drop-off Area");

        int rows=(AREA_COUNT+2)/3;
        float by=(top+28)+(rows-1)*41.0f+34.0f+26;

        dtxt("Desired Time (HH:MM)",(int)lx,(int)by,16,W_MUT); by+=20;
        field(sr_time,{lx,by,200,44},"08:30");
        dtxt("(+/- 30 min)",(int)(lx+218),(int)(by+12),14,W_MUT);
        by+=54;

        if (!err.empty()) { dtxt(err.c_str(),(int)lx,(int)by,15,W_ERR); by+=24; }

        if (btnP({lx,by,180,48},"Search")) {
            std::string t=sr_time.str();
            int th=-1,tm=-1;
            if (t.size()==5&&t[2]==':')
                try { th=std::stoi(t.substr(0,2)); tm=std::stoi(t.substr(3,2)); } catch(...){}
            if (th<0||th>23||tm<0||tm>59) err="Invalid time (HH:MM).";
            else if (sr_pu==sr_do)        err="Pickup and drop-off must differ.";
            else { sr_res=sys.searchRides(intToArea(sr_pu),intToArea(sr_do),t); go(Screen::SEARCH_RESULTS); }
        }
        if (backBtn()) go(Screen::PASSENGER_MENU);
    }

    void drawSearchResults() {
        ClearBackground(W_BG);
        hdr("Search Results");
        float x=50,y=86,rw=W-100;
        if (sr_res.empty()) dtxt("No rides found.",(int)x,(int)y,18,W_MUT);
        int n=0;
        for (auto& id:sr_res){
            if (n>=6) break;
            auto* r=sys.getRide(id);
            if (!r) continue;
            bool booked=false;
            for (auto& p:r->getPassengers()) if(p==uid){booked=true;break;}

            mbox(x,y,rw-220,120);
            dtxt(id.c_str(),(int)(x+14),(int)(y+10),18,WHITE);
            dtxt((areaToStr(r->getPickupArea())+" -> "+areaToStr(r->getDropOffArea())).c_str(),
                     (int)(x+14),(int)(y+38),15,WHITE);
            auto* v=sys.getVehicle(r->getVehicleId());
            std::string vi=v?v->getMake()+" "+v->getModel()+" ("+v->getType()+")":"N/A";
            dtxt(("Date: "+r->getRideDate()+"  Time: "+r->getRideTime()+
                      "  Seats: "+std::to_string(r->getSeatsLeft())+"  "+vi).c_str(),
                     (int)(x+14),(int)(y+66),13,W_PLBL);
            dtxt(("Pref: "+genderPrefToStr(r->getGenderPref())).c_str(),
                     (int)(x+14),(int)(y+90),13,W_PLBL);

            if (booked)
                dtxt("Booked",(int)(x+rw-200),(int)(y+50),17,W_OK);
            else if (r->getStatus()==RideStatus::OPEN){
                // light button with maroon text so it reads on the maroon box
                if (btn({x+rw-212,y+37,178,46},"Book Ride",W_FIELD,W_ACN,15))
                    if (!sys.bookRide(id,uid)) err="Cannot book (pref/seats).";
                    else { err=""; sr_res=sys.searchRides(intToArea(sr_pu),intToArea(sr_do),sr_time.str()); }
            } else dtxt("Full",(int)(x+rw-200),(int)(y+50),17,Color{255,180,80,255});

            y+=130; n++;
        }
        if (!err.empty()) dtxt(err.c_str(),(int)x,(int)(H-54),15,W_ERR);
        if (backBtn()) go(Screen::SEARCH_RIDE);
    }

    void drawEditProfile() {
        ClearBackground(W_BG);
        hdr("Edit Profile");
        auto* st=sys.getStudent(uid);
        if (!st){ go(Screen::ROLE_SELECT); return; }

        float x=50,y=86,iw=400,ih=46;
        dtxt(("ID: "+uid).c_str(),(int)x,(int)y,17,W_MUT); y+=36;

        auto lbl=[&](const char* s){ dtxt(s,(int)x,(int)y,15,W_MUT); y+=20; };
        lbl("Full Name");    field(ep_name, {x,y,iw,ih},"Name");          y+=ih+12;
        lbl("Phone (digits, max 11)");
                             field(ep_phone,{x,y,iw,ih},"Phone",true,11); y+=ih+12;
        dtxt("Gender",(int)x,(int)y,15,W_MUT); y+=20;
        if (btn({x,    y,194,40},"Male",   ep_gender==0?W_ACN:W_SEC, ep_gender==0?WHITE:W_TXT)) ep_gender=0;
        if (btn({x+200,y,194,40},"Female", ep_gender==1?W_ACN:W_SEC, ep_gender==1?WHITE:W_TXT)) ep_gender=1;
        y+=52;

        if (drv) {
            float rx=x+iw+60,ry=86;
            auto* v=sys.getVehicle(sys.getVehicleForDriver(uid));
            dtxt("Vehicle",(int)rx,(int)ry,18,W_TXT); ry+=32;
            if (v){
                dtxt(("Type: "+v->getType()).c_str(),(int)rx,(int)ry,14,W_MUT); ry+=24;
                auto vl=[&](const char* s){ dtxt(s,(int)rx,(int)ry,15,W_MUT); ry+=20; };
                vl("Make");  field(ep_vmake, {rx,ry,iw,ih},"Toyota");   ry+=ih+12;
                vl("Model"); field(ep_vmodel,{rx,ry,iw,ih},"Corolla");  ry+=ih+12;
                vl("Plate"); field(ep_vplate,{rx,ry,iw,ih},"KHI-0000"); ry+=ih+12;
                if (v->getType()=="Car"){ vl("Seats (1-6)"); field(ep_vcap,{rx,ry,iw,ih},"4"); }
                else dtxt("Seats: 1 (fixed)",(int)rx,(int)ry,14,W_MUT);
            } else dtxt("No vehicle on account.",(int)rx,(int)ry,15,W_ERR);
        }

        if (!ep_ok.empty()) { dtxt(ep_ok.c_str(),(int)x,(int)y,16,W_OK);  y+=28; }
        if (!err.empty())   { dtxt(err.c_str(),  (int)x,(int)y,16,W_ERR); y+=28; }

        if (btnP({x,y,210,48},"Save Changes")) {
            try {
                std::string nm=ep_name.str(),ph=ep_phone.str();
                if (nm.empty()) throw std::runtime_error("Name required.");
                if (ph.empty()) throw std::runtime_error("Phone required.");
                if ((int)ph.size()>11) throw std::runtime_error("Phone max 11 digits.");
                for (char c:ph) if(!isdigit((unsigned char)c)) throw std::runtime_error("Digits only.");
                Gender g=ep_gender==0?Gender::MALE:Gender::FEMALE;
                if (!sys.updateStudent(uid,nm,ph,g)) throw std::runtime_error("Update failed.");
                if (drv){
                    auto* v=sys.getVehicle(sys.getVehicleForDriver(uid));
                    if (v){
                        std::string mk=ep_vmake.str(),mdl=ep_vmodel.str(),pl=ep_vplate.str();
                        if (mk.empty()||mdl.empty()||pl.empty()) throw std::runtime_error("All vehicle fields required.");
                        int cap=v->getCapacity();
                        if (v->getType()=="Car"){
                            try { cap=ep_vcap.str().empty()?cap:std::stoi(ep_vcap.str()); }
                            catch(...){ throw std::runtime_error("Seats must be a number."); }
                            if (cap<1||cap>6) throw std::runtime_error("Seats 1-6.");
                        }
                        sys.updateVehicle(uid,mk,mdl,pl,cap);
                    }
                }
                ep_ok="Saved."; err="";
            } catch(std::runtime_error& e){ err=e.what(); ep_ok=""; }
        }

        y+=62;
        if (!ep_del){
            bool ov=CheckCollisionPointRec(GetMousePosition(),{x,y,230,38});
            DrawCircle((int)x+16,(int)y+19,14,ov?Color{240,60,60,255}:W_ERR);
            dtxt("!",(int)x+12,(int)y+9,20,WHITE);
            dtxt("Delete Account",(int)x+36,(int)y+10,17,ov?Color{240,60,60,255}:W_ERR);
            if (ov&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ep_del=true;
        } else {
            dtxt("Are you sure? Cannot be undone.",(int)x,(int)y,15,W_TXT); y+=28;
            if (btnD({x,    y,158,38},"Yes, Delete",14)){ sys.deleteStudent(uid); uid=""; go(Screen::ROLE_SELECT); return; }
            if (btnS({x+168,y,118,38},"Cancel",14)) ep_del=false;
        }
        if (backBtn()) go(drv?Screen::DRIVER_MENU:Screen::PASSENGER_MENU);
    }

public:
    explicit GUI3(CarpoolSystem& s):sys(s){}

    void run(){
        SetConfigFlags(FLAG_MSAA_4X_HINT);
        InitWindow(W,H,"IBA Carpool");
        g_font = LoadFontEx("ABeeZee-Regular.otf", 96, nullptr, 0);
        SetTextureFilter(g_font.texture, TEXTURE_FILTER_BILINEAR);
#ifdef _WIN32
        setAppIcon(GetWindowHandle());
#endif
        SetTargetFPS(60);
        while (!WindowShouldClose()&&!quit){
            BeginDrawing();
            switch(screen){
                case Screen::ROLE_SELECT:    drawRoleSelect();    break;
                case Screen::LOGIN:          drawLogin();         break;
                case Screen::REGISTER_FORM:  drawRegister();      break;
                case Screen::DRIVER_MENU:    drawDriverMenu();    break;
                case Screen::CREATE_RIDE:    drawCreateRide();    break;
                case Screen::CURRENT_RIDE:   drawCurrentRide();   break;
                case Screen::RIDE_HISTORY:   drawHistory();       break;
                case Screen::PASSENGER_MENU: drawPassengerMenu(); break;
                case Screen::SEARCH_RIDE:    drawSearchRide();    break;
                case Screen::SEARCH_RESULTS: drawSearchResults(); break;
                case Screen::EDIT_PROFILE:   drawEditProfile();   break;
            }
            xbtn(quit);
            EndDrawing();
        }
        UnloadFont(g_font);
        CloseWindow();
    }
};

void runGUI(CarpoolSystem& sys){ GUI3 g(sys); g.run(); }

#endif // USE_GUI
