#include "HienThi.h"
#include <stdint.h>
#include <Firebase_ESP_Client.h>
#define TFT_DARKRED tft.color565(139, 0, 0)
#define TFT_GRAY tft.color565(128, 128, 128)
#define TFT_LIGHTBLUE tft.color565(173, 216, 230)
bool daChuyenSangChonCay = false;
const unsigned long THOI_GIAN_CAP_NHAT_MAN_HINH = 8000;
// Constructor - Cáº£i tiáº¿n layout vÃ  kÃ­ch thÆ°á»›c nÃºt
HienThi::HienThi(int h, int c) : soHang(h),
                                 soCot(c),
                                 trangThaiHienThi(0),
                                 cheDoHienTai(CHE_DO_AUTO),
                                 daCham(false),
                                 diemChamX(0),
                                 diemChamY(0)
{
    // Khá»Ÿi táº¡o mÃ n hÃ¬nh TFT
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(MAU_NEN);

    // Khá»Ÿi táº¡o vector luongCay vá»›i kÃ­ch thÆ°á»›c soHang x soCot
    luongCay.resize(soHang);
    for (int i = 0; i < soHang; i++)
    {
        luongCay[i].resize(soCot, nullptr);
    }

    // Thiáº¿t láº­p luá»‘ng Ä‘ang chá»n máº·c Ä‘á»‹nh
    luongDangChon[0] = 0;
    luongDangChon[1] = 0;

    // KHá»žI Táº O CÃC NÃšT Vá»šI KÃCH THÆ¯á»šC VÃ€ Vá»Š TRÃ

    // NÃºt Menu (gÃ³c pháº£i trÃªn)
    nutMenu = {230, 210, 80, 25, "MENU", false};

    // NÃºt Quay láº¡i cho mÃ n hÃ¬nh chÃ­nh (gÃ³c trÃ¡i dÆ°á»›i)
    nutQuayLai = {10, 210, 80, 25, "<THOAT", false};
    // NÃºt ThoÃ¡t riÃªng cho giao diá»‡n MENU
    nutThoatMenu = {(tft.width() - 80) / 2, 200, 80, 25, "<THOAT", false};

    // NÃºt chuyá»ƒn luá»‘ng
    nutChonCay = {120, 210, 80, 25, "NEXT>", false};

    // CÃ¡c nÃºt Ä‘iá»u khiá»ƒn má»›i vá»›i spacing há»£p lÃ½
    nutAutoManual = {180, 120, 120, 35, "MANUAL", false};
    nutCaiDat = {50, 180, 100, 35, "CAI DAT", false};
    nutThongKe = {170, 180, 100, 35, "THONG KE", false};

    // CÃ¡c nÃºt Ä‘iá»u khiá»ƒn thiáº¿t bá»‹ (cho cháº¿ Ä‘á»™ manual) - cÄƒn Ä‘á»u
    nutBom = {40, 180, 80, 25, "BOM", false};
    nutQuat = {130, 180, 80, 25, "QUAT", false};
    nutDen = {220, 180, 80, 25, "DEN", false};

    // Khá»Ÿi táº¡o cÃ¡c nÃºt chá»n loáº¡i cÃ¢y vá»›i layout grid 2x3
    nutChonLoaiCay.resize(SO_LOAI_CAY);
    int startX = 40;
    int startY = 70;
    int buttonWidth = 100;
    int buttonHeight = 30;
    int spacingX = 120;
    int spacingY = 40;

    // Sáº¯p xáº¿p theo grid 2 cá»™t
    for (int i = 0; i < SO_LOAI_CAY; i++)
    {
        int row = i / 2;
        int col = i % 2;
        nutChonLoaiCay[i].x = startX + col * spacingX;
        nutChonLoaiCay[i].y = startY + row * spacingY;
        nutChonLoaiCay[i].chieuRong = buttonWidth;
        nutChonLoaiCay[i].chieuCao = buttonHeight;
        nutChonLoaiCay[i].dangDuocBam = false;
    }

    nutChonLoaiCay[0].nhan = "CA CHUA";
    nutChonLoaiCay[1].nhan = "XA LACH";
    nutChonLoaiCay[2].nhan = "HUNG QUE";
    nutChonLoaiCay[3].nhan = "BAC HA";
    nutChonLoaiCay[4].nhan = "DUA CHUOT";
    nutChonLoaiCay[5].nhan = "DAU TAY";

    // Thiáº¿t láº­p cáº£m á»©ng
    uint16_t calData[5] = {275, 3620, 264, 3532, 1};
    tft.setTouch(calData);
}

// Váº¼ HEADER Vá»šI GRADIENT Äáº¸P HÆ N
void HienThi::veHeader(const String &tieuDe, uint16_t mauChinh)
{
    // Váº½ gradient header vá»›i hiá»‡u á»©ng Ä‘áº¹p hÆ¡n
    for (int i = 0; i < 35; i++)
    {
        uint16_t mau;
        if (mauChinh == TFT_BLUE)
        {
            mau = tft.color565(0, 0, 200 - i * 4);
        }
        else if (mauChinh == TFT_GREEN)
        {
            mau = tft.color565(0, 200 - i * 4, 0);
        }
        else if (mauChinh == TFT_RED)
        {
            mau = tft.color565(200 - i * 4, 0, 0);
        }
        else
        {
            mau = tft.color565(150 - i * 2, 150 - i * 2, 200 - i * 3);
        }
        tft.drawFastHLine(0, i, 320, mau);
    }

    // ÄÆ°á»ng viá»n header
    tft.drawFastHLine(0, 35, 320, TFT_WHITE);

    // TiÃªu Ä‘á» cÄƒn giá»¯a
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    int textWidth = tieuDe.length() * 12;
    int x = (320 - textWidth) / 2;
    tft.setCursor(x, 14);
    tft.println(tieuDe);
}

// GÃ¡n thÃ´ng tin pháº§n cá»©ng
void HienThi::ganThongTinPhanCung(Adafruit_MCP23X17 *mcp,
                                  const int chanDHT_[2][2],
                                  const int chanDatAm_[2][2],
                                  const int chanAnhSang_[2][2],
                                  const int chanBomNuoc_[2][2],
                                  const int chanDen_[2][2],
                                  const int chanQuat_[2][2])
{

    mcpPtr = mcp;
    chanDHT = chanDHT_;
    chanDatAm = chanDatAm_;
    chanAnhSang = chanAnhSang_;
    chanBomNuoc = chanBomNuoc_;
    chanDen = chanDen_;
    chanQuat = chanQuat_;
}

// Váº¼ CARD THÃ”NG TIN Vá»šI SHADOW VÃ€ BORDER Äáº¸P
void HienThi::veCardThongTin(int x, int y, int w, int h)
{
    // Hiá»‡u á»©ng bÃ³ng
    tft.fillRoundRect(x + 2, y + 2, w, h, 8, TFT_DARKGREY);

    // Card chÃ­nh
    tft.fillRoundRect(x, y, w, h, 8, TFT_BLACK);

    // Viá»n vá»›i gradient
    tft.drawRoundRect(x, y, w, h, 8, TFT_CYAN);
    tft.drawRoundRect(x + 1, y + 1, w - 2, h - 2, 7, TFT_LIGHTBLUE);
}

// Cáº¢I TIáº¾N HÃ€M Váº¼ THANH TIáº¾N TRÃŒNH
void HienThi::veThanhTienTrinh(int x, int y, int width, int height, float giaTriHienTai, float giaTriToiDa, uint16_t mauThanh)
{
    // Viá»n
    tft.drawRoundRect(x, y, width, height, 3, TFT_WHITE);
    tft.fillRoundRect(x + 1, y + 1, width - 2, height - 2, 2, TFT_BLACK);

    // TÃ­nh toÃ¡n chiá»u rá»™ng thanh
    int chieuRongThanh = (int)((giaTriHienTai / giaTriToiDa) * (width - 4));
    if (chieuRongThanh > width - 4)
        chieuRongThanh = width - 4;
    if (chieuRongThanh < 0)
        chieuRongThanh = 0;

    // Váº½ thanh vá»›i gradient mÃ u theo má»©c Ä‘á»™
    for (int i = 0; i < chieuRongThanh; i++)
    {
        float ratio = (float)i / (width - 4);
        uint16_t mau;

        if (ratio < 0.3)
            mau = TFT_RED;
        else if (ratio < 0.6)
            mau = TFT_YELLOW;
        else
            mau = TFT_GREEN;

        tft.drawFastVLine(x + 2 + i, y + 2, height - 4, mau);
    }
}

// Váº¼ GIAO DIá»†N CHÃNH
void HienThi::veGiaoDienChinh()
{
    tft.fillScreen(TFT_BLACK);

    // Reset láº¡i vá»‹ trÃ­ nÃºt
    nutQuayLai = {10, 210, 80, 25, "<THOAT", false};
    nutChonCay = {120, 210, 80, 25, "NEXT>", false};
    nutMenu = {230, 210, 80, 25, "MENU", false};

    nutBom.y = 180;
    nutQuat.y = 180;
    nutDen.y = 180;

    // TiÃªu Ä‘á»
    veHeader("VUON THONG MINH", TFT_CYAN);

    // ThÃ´ng tin cháº¿ Ä‘á»™
    tft.fillRect(10, 40, 200, 20, TFT_BLACK); // XÃ³a ná»n dÃ²ng trÆ°á»›c
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 40);
    tft.print("Che do: ");
    tft.setTextColor(cheDoHienTai == CHE_DO_AUTO ? TFT_GREEN : TFT_ORANGE);
    tft.print(cheDoHienTai == CHE_DO_AUTO ? "TU DONG" : "THU CONG");

    // Vá»‹ trÃ­ luá»‘ng Ä‘ang chá»n
    int hang, cot;
    layLuongDangChon(hang, cot);
    HeThongVuon *cay = luongCay[hang][cot];

    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(2);
    tft.setCursor(10, 60);
    tft.print("LUONG [" + String(hang) + "][" + String(cot) + "]");

    if (cay == nullptr)
    {
        // Náº¿u chÆ°a trá»“ng cÃ¢y
        tft.setTextColor(TFT_GREEN);
        tft.setCursor(10, 90);
        tft.print("CHUA CO CAY TAI LUONG NAY");

        veNut(nutMenu);
        veNut(nutChonCay);
        veNut(nutQuayLai);
        return;
    }

    // Loáº¡i cÃ¢y
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(10, 80);
    tft.print("Loai: ");
    tft.print(cay->layCauHinhCay().layTen());

    // Hiá»ƒn thá»‹ cáº£m biáº¿n
    int y = 100;
    veBieuTuongDoAm(10, y);
    tft.setCursor(30, y);
    tft.setTextColor(TFT_CYAN);
    tft.print(cay->layDoAmDat(), 1);
    tft.print("%");

    y += 25;
    veBieuTuongAnhSang(10, y);
    tft.setCursor(30, y);
    tft.setTextColor(TFT_YELLOW);
    tft.print(cay->layAnhSang(), 1);
    tft.print(" lux");

    y += 25;
    veBieuTuongNhietDo(10, y);
    tft.setCursor(30, y);
    float nhiet = cay->layNhietDo();
    tft.setTextColor(nhiet > 35 ? TFT_RED : (nhiet > 25 ? TFT_ORANGE : TFT_CYAN));
    tft.print(nhiet, 1);
    tft.print("C");

    // NÃºt Ä‘iá»u khiá»ƒn thiáº¿t bá»‹ (náº¿u á»Ÿ cháº¿ Ä‘á»™ thá»§ cÃ´ng)
    if (cheDoHienTai == CHE_DO_MANUAL)
    {
        veNut(nutBom);
        veNut(nutQuat);
        veNut(nutDen);
    }

    // NÃºt Ä‘iá»u hÆ°á»›ng chung
    veNut(nutMenu);
    veNut(nutChonCay);
    veNut(nutQuayLai);
}

// Cáº¢I TIáº¾N GIAO DIá»†N MENU
void HienThi::veGiaoDienMenu()
{
    tft.fillScreen(TFT_BLACK);
    veHeader("MENU CHINH", TFT_GREEN);

    tft.setTextSize(2);
    tft.setTextColor(TFT_ORANGE);
    tft.setCursor(10, 40);
    tft.print("Che do: ");
    tft.print(cheDoHienTai == CHE_DO_AUTO ? "TU DONG" : "THU CONG");

    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 60);
    tft.print("Chon chuc nang:");

    // Äáº·t vá»‹ trÃ­ Ä‘á»u 5 nÃºt
    int nutW = 100;
    int nutH = 28;
    int nutX = (320 - nutW) / 2;
    int startY = 80;
    int space = 4;

    nutAutoManual = {nutX, startY + 0 * (nutH + space), nutW, nutH, "MANUAL", false};
    nutThongKe = {nutX, startY + 1 * (nutH + space), nutW, nutH, "THONG KE", false};
    nutTrongCay = {nutX, startY + 2 * (nutH + space), nutW, nutH, "TRONG CAY", false};
    nutCaiDat = {nutX, startY + 3 * (nutH + space), nutW, nutH, "CAI DAT", false};
    nutQuayLai = {nutX, startY + 4 * (nutH + space), nutW, nutH, "THOAT", false};

    // Váº½ nÃºt
    veNut(nutAutoManual);
    veNut(nutThongKe);
    veNut(nutTrongCay);
    veNut(nutCaiDat);
    veNut(nutQuayLai);
}

// GIAO DIá»†N CHá»ŒN LOáº I CÃ‚Y
void HienThi::veGiaoDienChonLoaiCay()
{
    tft.fillScreen(TFT_BLACK);

    veHeader("CHON LOAI CAY", TFT_GREEN);

    // Hiá»ƒn thá»‹ vá»‹ trÃ­ Ä‘ang chá»n
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(2);
    tft.setCursor(20, 45);
    tft.print("Luong [");
    tft.print(luongDangChon[0]);
    tft.print("][");
    tft.print(luongDangChon[1]);
    tft.print("]");

    // Náº¿u Ä‘Ã£ trá»“ng cÃ¢y thÃ¬ hiá»ƒn thá»‹ tÃªn
    HeThongVuon *cay = layHeThongVuon(luongDangChon[0], luongDangChon[1]);
    if (cay != nullptr)
    {
        tft.setTextColor(TFT_CYAN);
        tft.setCursor(20, 70); // hiá»ƒn thá»‹ táº¡i dÃ²ng riÃªng
        tft.print("Da co: ");
        tft.print(cay->layCauHinhCay().layTen());
    }

    int startX = 40;
    int startY = 100;
    int buttonWidth = 100;
    int buttonHeight = 30;
    int spacingX = 120;
    int spacingY = 35;

    for (int i = 0; i < SO_LOAI_CAY; i++)
    {
        int row = i / 2;
        int col = i % 2;
        nutChonLoaiCay[i].x = startX + col * spacingX;
        nutChonLoaiCay[i].y = startY + row * spacingY;
        nutChonLoaiCay[i].chieuRong = buttonWidth;
        nutChonLoaiCay[i].chieuCao = buttonHeight;
        nutChonLoaiCay[i].dangDuocBam = false;
        veNut(nutChonLoaiCay[i]);
    }

    // NÃºt quay láº¡i
    nutQuayLai = {10, 210, 80, 25, "THOAT", false};
    veNut(nutQuayLai);
}

// HÃ€M CHÃNH Váº¼ GIAO DIá»†N
void HienThi::veGiaoDien()
{
    switch (trangThaiHienThi)
    {
    case 0: // Giao diá»‡n chÃ­nh
        veGiaoDienChinh();
        break;

    case 1: // Menu
        veGiaoDienMenu();
        break;

    case 2: // Äiá»u khiá»ƒn
        veManHinhDieuKhien();
        break;

    case 3: // Chá»n loáº¡i cÃ¢y
        veGiaoDienChonLoaiCay();
        break;

    case 4: // CÃ i Ä‘áº·t
        tft.fillScreen(TFT_BLACK);
        veHeader("CAI DAT", TFT_YELLOW);

        tft.setTextColor(TFT_CYAN);
        tft.setTextSize(2);
        tft.setCursor(20, 60);
        tft.println("Cac tuy chon cai dat:");

        tft.setTextColor(TFT_WHITE);
        tft.setCursor(20, 80);
        tft.println("- Nguong cam bien");
        tft.setCursor(20, 95);
        tft.println("- Thoi gian tuoi nuoc");
        tft.setCursor(20, 125);
        tft.println("- Reset he thong");

        tft.setTextColor(TFT_YELLOW);
        tft.setCursor(20, 150);
        tft.println("Tinh nang dang phat trien...");

        veNut(nutQuayLai);
        break;

    case 5: // Thá»‘ng kÃª
        tft.fillScreen(TFT_BLACK);
        veHeader("THONG KE", TFT_RED);

        // TÃ­nh toÃ¡n thá»‘ng kÃª
        int soCayDaTrong = 0;
        int soCayCanTuoi = 0;
        int soBomDangBat = 0;

        for (int i = 0; i < soHang; i++)
        {
            for (int j = 0; j < soCot; j++)
            {
                if (luongCay[i][j] != nullptr)
                {
                    soCayDaTrong++;
                    if (luongCay[i][j]->layDoAmDat() < luongCay[i][j]->layCauHinhCay().layNguongDoAmDat())
                    {
                        soCayCanTuoi++;
                    }
                    if (luongCay[i][j]->layTrangThaiBom())
                    {
                        soBomDangBat++;
                    }
                }
            }
        }

        tft.setTextColor(TFT_CYAN);
        tft.setTextSize(2);
        tft.setCursor(20, 60);
        tft.println("Thong ke tong quan:");

        tft.setTextColor(TFT_WHITE);
        tft.setCursor(20, 80);
        tft.print("So cay da trong: ");
        tft.setTextColor(TFT_GREEN);
        tft.print(soCayDaTrong);
        tft.setTextColor(TFT_WHITE);
        tft.print("/");
        tft.print(soHang * soCot);

        tft.setCursor(20, 100);
        tft.print("So cay can tuoi: ");
        tft.setTextColor(soCayCanTuoi > 0 ? TFT_YELLOW : TFT_GREEN);
        tft.print(soCayCanTuoi);

        tft.setTextColor(TFT_WHITE);
        tft.setCursor(20, 120);
        tft.print("So bom dang bat: ");
        tft.setTextColor(TFT_CYAN);
        tft.print(soBomDangBat);

        // Hiá»‡u suáº¥t há»‡ thá»‘ng
        float hieuSuat = (float)soCayDaTrong / (soHang * soCot) * 100;
        tft.setTextColor(TFT_YELLOW);
        tft.setCursor(20, 150);
        tft.println("Hieu suat he thong:");

        tft.setTextColor(TFT_WHITE);
        tft.setCursor(20, 170);
        tft.print("Su dung: ");
        tft.setTextColor(hieuSuat > 70 ? TFT_GREEN : (hieuSuat > 40 ? TFT_YELLOW : TFT_RED));
        tft.print(hieuSuat, 1);
        tft.print("%");

        veThanhTienTrinh(20, 190, 200, 15, hieuSuat, 100, TFT_GREEN);

        veNut(nutQuayLai);
        break;
    }
}

// Cáº¬P NHáº¬T Xá»¬ LÃ TOUCH Äá»‚ PHáº¢N Há»’I Vá»šI LAYOUT Má»šI
void HienThi::capNhatTrangThaiNut()
{
    uint16_t x, y;
    bool pressed = tft.getTouch(&x, &y);

    if (!pressed)
    {
        nutMenu.dangDuocBam = false;
        nutChonCay.dangDuocBam = false;
        nutQuayLai.dangDuocBam = false;
        nutAutoManual.dangDuocBam = false;
        nutCaiDat.dangDuocBam = false;
        nutThongKe.dangDuocBam = false;

        for (int i = 0; i < SO_LOAI_CAY; i++)
        {
            nutChonLoaiCay[i].dangDuocBam = false;
        }

        daCham = false; // Reset Ä‘á»ƒ cho phÃ©p cháº¡m láº¡i
        return;
    }

    if (daCham && abs(x - diemChamX) < 10 && abs(y - diemChamY) < 10)
    {
        return;
    }

    daCham = true;
    diemChamX = x;
    diemChamY = y;

    switch (trangThaiHienThi)
    {
    case 0: // MÃ n hÃ¬nh chÃ­nh
        if (kiemTraNut(nutMenu, x, y))
        {
            trangThaiHienThi = 1;
            veGiaoDien();
        }
        else if (kiemTraNut(nutChonCay, x, y))
        {
            luongDangChon[1]++;
            if (luongDangChon[1] >= soCot)
            {
                luongDangChon[1] = 0;
                luongDangChon[0]++;
                if (luongDangChon[0] >= soHang)
                {
                    luongDangChon[0] = 0;
                }
            }
            veGiaoDien();
        }
        else if (kiemTraNut(nutQuayLai, x, y))
        {
            thongBaoCanhBao("He thong dang hoat dong!");
        }
        else if (cheDoHienTai == CHE_DO_MANUAL)
        {
            xuLyDieuKhienThuCong(x, y); // Xá»­ lÃ½ cÃ¡c nÃºt BÆ M / QUáº T / ÄÃˆN táº¡i mÃ n chÃ­nh
        }
        break;

    case 1: // MENU
        if (kiemTraNut(nutAutoManual, x, y))
        {
            cheDoHienTai = (cheDoHienTai == CHE_DO_AUTO) ? CHE_DO_MANUAL : CHE_DO_AUTO;
            for (int i = 0; i < soHang; i++)
            {
                for (int j = 0; j < soCot; j++)
                {
                    if (luongCay[i][j] != nullptr)
                    {
                        bool laTuDong = (cheDoHienTai == CHE_DO_AUTO);
                        luongCay[i][j]->chuyenCheDo(laTuDong);

                        if (!laTuDong)
                        {
                            // Náº¿u vá»«a chuyá»ƒn sang thá»§ cÃ´ng thÃ¬ táº¯t háº¿t relay
                            luongCay[i][j]->datTrangThaiBom(false);
                            luongCay[i][j]->datTrangThaiDen(false);
                            luongCay[i][j]->datTrangThaiQuat(false);
                        }
                    }
                }
            }
            thongBaoCanhBao(cheDoHienTai == CHE_DO_AUTO ? "TU DONG" : "THU CONG");
            veGiaoDien();
        }
        else if (kiemTraNut(nutThongKe, x, y))
        {
            trangThaiHienThi = 5;
            veGiaoDien();
        }
        else if (kiemTraNut(nutTrongCay, x, y))
        {
            trangThaiHienThi = 3;
            daChuyenSangChonCay = true;
            veGiaoDien();
        }
        else if (kiemTraNut(nutCaiDat, x, y))
        {
            // Sau nÃ y thÃªm giao diá»‡n cÃ i Ä‘áº·t
            thongBaoCanhBao("Tinh nang dang phat trien!");
        }
        else if (kiemTraNut(nutQuayLai, x, y))
        {
            trangThaiHienThi = 0;
            veGiaoDien();
        }
        break;

    case 2: // MÃ n hÃ¬nh Ä‘iá»u khiá»ƒn
        if (cheDoHienTai == CHE_DO_MANUAL)
        {
            xuLyDieuKhienThuCong(x, y);
        }
        if (kiemTraNut(nutQuayLai, x, y))
        {
            trangThaiHienThi = 1;
            veGiaoDien();
        }
        break;

    case 3: // Giao diá»‡n chá»n loáº¡i cÃ¢y
        if (kiemTraNut(nutQuayLai, x, y))
        {
            trangThaiHienThi = 1; // Vá» MENU
            veGiaoDien();
        }
        else
        {
            for (int i = 0; i < SO_LOAI_CAY; i++)
            {
                if (kiemTraNut(nutChonLoaiCay[i], x, y))
                {
                    // Trá»“ng cÃ¢y táº¡i vá»‹ trÃ­ Ä‘ang chá»n
                    trongCayTaiViTri(luongDangChon[0], luongDangChon[1], i);

                    // Äáº£m báº£o luongDangChon váº«n trá» Ä‘Ãºng vá»‹ trÃ­ vá»«a trá»“ng
                    // (ÄÃ£ Ä‘Æ°á»£c cáº­p nháº­t trong hÃ m trongCayTaiViTri)

                    trangThaiHienThi = 0; // Vá» giao diá»‡n chÃ­nh
                    veGiaoDien();
                    break;
                }
            }
        }
        break;

    case 4: // CÃ i Ä‘áº·t
    case 5: // Thá»‘ng kÃª
        if (kiemTraNut(nutQuayLai, x, y))
        {
            trangThaiHienThi = 1;
            veGiaoDien();
        }
        break;
    }
}

// Xá»¬ LÃ ÄIá»€U KHIá»‚N THá»¦ CÃ”NG
void HienThi::xuLyDieuKhienThuCong(int x, int y)
{
    int hang = luongDangChon[0];
    int cot = luongDangChon[1];

    if (luongCay[hang][cot] == nullptr)
    {
        thongBaoCanhBao("Khong co cay tai luong nay!");
        return;
    }

    bool coThayDoi = false;

    if (kiemTraNut(nutBom, x, y))
    {
        bool trangThaiMoi = !luongCay[hang][cot]->layTrangThaiBom();
        luongCay[hang][cot]->datTrangThaiBom(trangThaiMoi);
        thongBaoCanhBao(trangThaiMoi ? "Bat bom" : "Tat bom");
        coThayDoi = true;

        // Gá»­i Firebase
        if (fbdoPtr != nullptr && Firebase.ready())
        {
            String gardenId = "gardenId" + String(hang * 2 + cot + 1);
            Firebase.RTDB.setInt(fbdoPtr, gardenId + "/mayBom", trangThaiMoi ? 1 : 0);
        }
    }
    else if (kiemTraNut(nutQuat, x, y))
    {
        bool trangThaiMoi = !luongCay[hang][cot]->layTrangThaiQuat();
        luongCay[hang][cot]->datTrangThaiQuat(trangThaiMoi);
        thongBaoCanhBao(trangThaiMoi ? "Bat quat" : "Tat quat");
        coThayDoi = true;

        if (fbdoPtr != nullptr && Firebase.ready())
        {
            String gardenId = "gardenId" + String(hang * 2 + cot + 1);
            Firebase.RTDB.setInt(fbdoPtr, gardenId + "/quat", trangThaiMoi ? 1 : 0);
        }
    }
    else if (kiemTraNut(nutDen, x, y))
    {
        bool trangThaiMoi = !luongCay[hang][cot]->layTrangThaiDen();
        luongCay[hang][cot]->datTrangThaiDen(trangThaiMoi);
        thongBaoCanhBao(trangThaiMoi ? "Bat den" : "Tat den");
        coThayDoi = true;

        if (fbdoPtr != nullptr && Firebase.ready())
        {
            String gardenId = "gardenId" + String(hang * 2 + cot + 1);
            Firebase.RTDB.setInt(fbdoPtr, gardenId + "/den", trangThaiMoi ? 1 : 0);
        }
    }
    else if (kiemTraNut(nutChonCay, x, y))
    {
        luongDangChon[1]++;
        if (luongDangChon[1] >= soCot)
        {
            luongDangChon[1] = 0;
            luongDangChon[0]++;
            if (luongDangChon[0] >= soHang)
            {
                luongDangChon[0] = 0;
            }
        }
        thongBaoCanhBao("Chuyen luong");
        coThayDoi = true;
    }

    if (coThayDoi)
    {
        veGiaoDien(); // Quay láº¡i Ä‘Ãºng giao diá»‡n hiá»‡n táº¡i
    }
}

// MÃ€N HÃŒNH ÄIá»€U KHIá»‚N THá»¦ CÃ”NG
void HienThi::veManHinhDieuKhien()
{
    tft.fillScreen(TFT_BLACK);

    veHeader("DIEU KHIEN THU CONG", TFT_RED);

    int hang = luongDangChon[0];
    int cot = luongDangChon[1];

    // Hiá»ƒn thá»‹ thÃ´ng tin luá»‘ng Ä‘ang chá»n
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(1);
    tft.setCursor(20, 45);
    tft.print("Luong [");
    tft.print(hang);
    tft.print("][");
    tft.print(cot);
    tft.print("]");

    if (luongCay[hang][cot] != nullptr)
    {
        tft.setTextColor(TFT_CYAN);
        tft.print(" - ");
        tft.print(luongCay[hang][cot]->layCauHinhCay().layTen());

        // Card thÃ´ng tin cáº£m biáº¿n
        veCardThongTin(10, 65, 300, 120);

        int yPos = 75;
        int spacing = 25;

        // Hiá»ƒn thá»‹ thÃ´ng tin cáº£m biáº¿n
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);

        // Äá»™ áº©m Ä‘áº¥t
        veBieuTuongDoAm(20, yPos);
        tft.setCursor(40, yPos);
        tft.print("Do am: ");
        tft.setTextColor(TFT_CYAN);
        tft.print(luongCay[hang][cot]->layDoAmDat(), 1);
        tft.print("%");

        // Nhiá»‡t Ä‘á»™
        yPos += spacing;
        veBieuTuongNhietDo(20, yPos);
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(40, yPos);
        tft.print("Nhiet do: ");
        tft.setTextColor(TFT_RED);
        tft.print(luongCay[hang][cot]->layNhietDo(), 1);
        tft.print("C");

        // Ãnh sÃ¡ng
        yPos += spacing;
        veBieuTuongAnhSang(20, yPos);
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(40, yPos);
        tft.print("Anh sang: ");
        tft.setTextColor(TFT_YELLOW);
        tft.print(luongCay[hang][cot]->layAnhSang(), 1);
        tft.print(" lux");

        // Tráº¡ng thÃ¡i thiáº¿t bá»‹ hiá»‡n táº¡i
        yPos += spacing;
        tft.setTextColor(TFT_LIGHTBLUE);
        tft.setCursor(20, yPos);
        tft.print("Trang thai hien tai:");

        yPos += 15;
        int xPos = 20;

        // Hiá»ƒn thá»‹ tráº¡ng thÃ¡i bÆ¡m
        tft.fillRoundRect(xPos, yPos, 60, 15, 3,
                          luongCay[hang][cot]->layTrangThaiBom() ? TFT_GREEN : TFT_RED);
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(xPos + 5, yPos + 3);
        tft.print("BOM: ");
        tft.print(luongCay[hang][cot]->layTrangThaiBom() ? "ON" : "OFF");

        xPos += 70;
        // Hiá»ƒn thá»‹ tráº¡ng thÃ¡i quáº¡t
        tft.fillRoundRect(xPos, yPos, 60, 15, 3,
                          luongCay[hang][cot]->layTrangThaiQuat() ? TFT_GREEN : TFT_RED);
        tft.setCursor(xPos + 5, yPos + 3);
        tft.print("QUAT: ");
        tft.print(luongCay[hang][cot]->layTrangThaiQuat() ? "ON" : "OFF");

        xPos += 70;
        // Hiá»ƒn thá»‹ tráº¡ng thÃ¡i Ä‘Ã¨n
        tft.fillRoundRect(xPos, yPos, 60, 15, 3,
                          luongCay[hang][cot]->layTrangThaiDen() ? TFT_GREEN : TFT_RED);
        tft.setCursor(xPos + 5, yPos + 3);
        tft.print("DEN: ");
        tft.print(luongCay[hang][cot]->layTrangThaiDen() ? "ON" : "OFF");
    }
    else
    {
        tft.setTextColor(TFT_GRAY);
        tft.setTextSize(2);
        tft.setCursor(80, 120);
        tft.println("KHONG CO CAY");
    }

    // CÃ¡c nÃºt Ä‘iá»u khiá»ƒn
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(20, 200);
    tft.println("Dieu khien thiet bi:");

    // Váº½ cÃ¡c nÃºt Ä‘iá»u khiá»ƒn vá»›i vá»‹ trÃ­ má»›i
    nutBom.y = 220;
    nutQuat.y = 220;
    nutDen.y = 220;
    nutChonCay.y = 260;

    veNut(nutBom);
    veNut(nutQuat);
    veNut(nutDen);
    veNut(nutChonCay);
    veNut(nutQuayLai);

    // HÆ°á»›ng dáº«n sá»­ dá»¥ng
    tft.setTextColor(TFT_LIGHTBLUE);
    tft.setTextSize(2);
    tft.setCursor(10, 290);
    tft.println("Bat/tat thiet bi");
}

// Váº¼ CÃC BIá»‚U TÆ¯á»¢NG Cáº¢M BIáº¾N
void HienThi::veBieuTuongDoAm(int x, int y)
{
    // Váº½ biá»ƒu tÆ°á»£ng giá»t nÆ°á»›c
    tft.fillCircle(x + 8, y + 10, 5, TFT_BLUE);
    tft.fillTriangle(x + 8, y + 2, x + 3, y + 10, x + 13, y + 10, TFT_BLUE);
}

void HienThi::veBieuTuongNhietDo(int x, int y)
{
    // Váº½ biá»ƒu tÆ°á»£ng nhiá»‡t káº¿
    tft.drawRoundRect(x + 4, y, 6, 14, 3, TFT_RED);
    tft.fillCircle(x + 7, y + 16, 3, TFT_RED);
    tft.drawFastVLine(x + 7, y + 2, 10, TFT_RED);
}

void HienThi::veBieuTuongAnhSang(int x, int y)
{
    // Váº½ biá»ƒu tÆ°á»£ng máº·t trá»i
    tft.fillCircle(x + 8, y + 8, 5, TFT_YELLOW);
    // CÃ¡c tia sÃ¡ng
    for (int i = 0; i < 8; i++)
    {
        float angle = i * 45 * PI / 180;
        int x1 = x + 8 + cos(angle) * 7;
        int y1 = y + 8 + sin(angle) * 7;
        int x2 = x + 8 + cos(angle) * 10;
        int y2 = y + 8 + sin(angle) * 10;
        tft.drawLine(x1, y1, x2, y2, TFT_YELLOW);
    }
}

// Váº¼ NÃšT Vá»šI HIá»†U á»¨NG
void HienThi::veNut(ThongTinNut nut)
{
    // Váº½ ná»n vÃ  viá»n nÃºt
    tft.fillRoundRect(nut.x, nut.y, nut.chieuRong, nut.chieuCao, 5, TFT_CYAN);
    tft.drawRoundRect(nut.x, nut.y, nut.chieuRong, nut.chieuCao, 5, TFT_BLUE);

    // Giáº£m kÃ­ch thÆ°á»›c chá»¯ Ä‘á»ƒ trÃ¡nh trÃ n
    tft.setTextSize(1); // Nhá» hÆ¡n, gá»n hÆ¡n
    tft.setTextColor(TFT_BLACK);

    // TÃ­nh chiá»u rá»™ng/chiá»u cao chá»¯
    int charWidth = 6;  // Má»—i kÃ½ tá»± ~6px á»Ÿ size 1
    int charHeight = 8; // Chiá»u cao á»Ÿ size 1
    int textWidth = nut.nhan.length() * charWidth;

    // CÄƒn giá»¯a chá»¯
    int textX = nut.x + (nut.chieuRong - textWidth) / 2;
    int textY = nut.y + (nut.chieuCao - charHeight) / 2;

    tft.setCursor(textX, textY);
    tft.print(nut.nhan);
}

// KIá»‚M TRA NÃšT ÄÆ¯á»¢C NHáº¤N
bool HienThi::kiemTraNut(ThongTinNut nut, int x, int y)
{
    if (x >= nut.x && x <= nut.x + nut.chieuRong &&
        y >= nut.y && y <= nut.y + nut.chieuCao)
    {
        nut.dangDuocBam = true;
        return true;
    }
    return false;
}

// THÃ”NG BÃO Cáº¢NH BÃO
void HienThi::thongBaoCanhBao(const String &thongDiep)
{
    int popupW = 200;
    int popupH = 60;
    int popupX = (tft.width() - popupW) / 2;
    int popupY = 80;
    int maxPixel = popupW - 20; // Padding trÃ¡i/pháº£i

    // Shadow + ná»n
    tft.fillRoundRect(popupX + 3, popupY + 3, popupW, popupH, 8, TFT_DARKGREY);
    tft.fillRoundRect(popupX, popupY, popupW, popupH, 8, TFT_ORANGE);
    tft.drawRoundRect(popupX, popupY, popupW, popupH, 8, TFT_RED);

    tft.setTextSize(2);
    tft.setTextColor(TFT_BLACK);

    // TÃ¡ch chuá»—i thÃ nh tá»«
    std::vector<String> tu;
    int start = 0;
    for (int i = 0; i <= thongDiep.length(); i++)
    {
        if (i == thongDiep.length() || thongDiep[i] == ' ')
        {
            tu.push_back(thongDiep.substring(start, i));
            start = i + 1;
        }
    }

    String dong1 = "", dong2 = "";
    int width1 = 0, width2 = 0;

    // Gá»™p tá»« vÃ o dÃ²ng 1
    for (size_t i = 0; i < tu.size(); i++)
    {
        String word = tu[i];
        int w = word.length() * 12 + (dong1 == "" ? 0 : 12); // Khoáº£ng tráº¯ng

        if (width1 + w <= maxPixel)
        {
            dong1 += (dong1 == "" ? "" : " ") + word;
            width1 += w;
        }
        else if (width2 + w <= maxPixel)
        {
            dong2 += (dong2 == "" ? "" : " ") + word;
            width2 += w;
        }
        else
        {
            dong2 += "..."; // Náº¿u quÃ¡ dÃ i thÃ¬ rÃºt gá»n
            break;
        }
    }

    // Hiá»ƒn thá»‹ dÃ²ng 1
    int textX1 = popupX + (popupW - dong1.length() * 12) / 2;
    tft.setCursor(textX1, popupY + 15);
    tft.print(dong1);

    // Hiá»ƒn thá»‹ dÃ²ng 2 náº¿u cÃ³
    if (dong2.length() > 0)
    {
        int textX2 = popupX + (popupW - dong2.length() * 12) / 2;
        tft.setCursor(textX2, popupY + 35);
        tft.print(dong2);
    }

    delay(1500);
    veGiaoDien();
}

void HienThi::chuyenDoiCheDoAutoManual()
{
    cheDoHienTai = (cheDoHienTai == CHE_DO_AUTO) ? CHE_DO_MANUAL : CHE_DO_AUTO;

    String thongBao = "Chuyen sang che do ";
    thongBao += (cheDoHienTai == CHE_DO_AUTO) ? "TU DONG" : "THU CONG";
    thongBaoCanhBao(thongBao);

    // Gá»­i currentMode lÃªn Firebase náº¿u Ä‘Ã£ cÃ³ fbdo
    if (fbdoPtr != nullptr && Firebase.ready())
    {
        String modeStr = (cheDoHienTai == CHE_DO_AUTO) ? "auto" : "manual";
        if (!Firebase.RTDB.setString(fbdoPtr, "currentMode", modeStr))
        {
            Serial.print("Loi gui mode len Firebase: ");
            Serial.println(fbdoPtr->errorReason());
        }
        else
        {
            Serial.print("Cap nhat mode thanh cong: ");
            Serial.println(modeStr);
        }
    }
}

// TRá»’NG CÃ‚Y Táº I Vá»Š TRÃ
void HienThi::trongCayTaiViTri(int hang, int cot, int loaiCay)
{
    if (hang >= 0 && hang < soHang && cot >= 0 && cot < soCot)
    {
        // XÃ³a cÃ¢y cÅ© náº¿u cÃ³
        if (luongCay[hang][cot] != nullptr)
        {
            delete luongCay[hang][cot];
        }

        // Táº¡o cáº£m biáº¿n cho vá»‹ trÃ­ tÆ°Æ¡ng á»©ng
        CamBien camDoAm(chanDatAm[hang][cot], 1, 0, 75, 0, 4095);
        CamBien camAnhSang(chanAnhSang[hang][cot], 1, 0, 40000, 4095, 0);
        CamBien camNhiet(chanDHT[hang][cot], 2);

        // Táº¡o há»‡ thá»‘ng vÆ°á»n má»›i vá»›i cáº£m biáº¿n
        HeThongVuon *cayMoi = new HeThongVuon(
            *mcpPtr,
            camNhiet,
            camDoAm,
            camAnhSang,
            chanBomNuoc[hang][cot],
            chanDen[hang][cot],
            chanQuat[hang][cot]);

        cayMoi->batDau();
        cayMoi->datTrangThaiBom(false);
        cayMoi->datTrangThaiDen(false);
        cayMoi->datTrangThaiQuat(false);

        // Thiáº¿t láº­p loáº¡i cÃ¢y
        CauHinhCay cauHinh = CauHinhCay::layCauHinhMacDinh(static_cast<LoaiCay>(loaiCay));
        cayMoi->thietLapCauHinhCay(cauHinh);

        // GÃ¡n há»‡ thá»‘ng vÃ o luá»‘ng
        thietLapLuongCay(hang, cot, cayMoi);

        // Hiá»ƒn thá»‹ thÃ´ng bÃ¡o
        String thongBao = "Trong ";
        thongBao += nutChonLoaiCay[loaiCay].nhan;
        thongBao += " tai [" + String(hang) + "][" + String(cot) + "]";
        thongBaoCanhBao(thongBao);

        // Vá» giao diá»‡n chÃ­nh
        trangThaiHienThi = 0;
        veGiaoDien();
    }
}

// Cáº¬P NHáº¬T Dá»® LIá»†U Tá»ª Cáº¢M BIáº¾N
void HienThi::capNhatDuLieuCamBien(int hang, int cot, float doAmDat, float nhietDo, float anhSang)
{
    if (hang >= 0 && hang < soHang && cot >= 0 && cot < soCot && luongCay[hang][cot] != nullptr)
    {
        luongCay[hang][cot]->capNhatDuLieuCamBien(doAmDat, nhietDo, anhSang);

        // Chá»‰ cáº­p nháº­t giao diá»‡n náº¿u Ä‘ang hiá»ƒn thá»‹ luá»‘ng nÃ y
        if (hang == luongDangChon[0] && cot == luongDangChon[1] && trangThaiHienThi == 0)
        {
            veGiaoDien();
        }
    }
}

// Xá»¬ LÃ LOGIC Tá»° Äá»˜NG
void HienThi::xuLyLogicTuDong()
{
    if (cheDoHienTai == CHE_DO_AUTO)
    {
        for (int i = 0; i < soHang; i++)
        {
            for (int j = 0; j < soCot; j++)
            {
                if (luongCay[i][j] != nullptr)
                {
                    luongCay[i][j]->xuLyLogicChamSoc();
                }
            }
        }
    }
}

// Láº¤Y TRáº NG THÃI Há»† THá»NG
int HienThi::layTrangThaiHienThi() const
{
    return trangThaiHienThi;
}

CheDoBoSung HienThi::layCheDoHienTai() const
{
    return cheDoHienTai;
}

void HienThi::datCheDoHienTai(CheDoBoSung cheDo)
{
    cheDoHienTai = cheDo;
}

// Láº¤Y THÃ”NG TIN LUá»NG ÄANG CHá»ŒN
void HienThi::layLuongDangChon(int &hang, int &cot) const
{
    hang = luongDangChon[0];
    cot = luongDangChon[1];
}

void HienThi::datLuongDangChon(int hang, int cot)
{
    if (hang >= 0 && hang < soHang && cot >= 0 && cot < soCot)
    {
        luongDangChon[0] = hang;
        luongDangChon[1] = cot;
    }
}

// THIáº¾T Láº¬P LUá»NG CÃ‚Y
void HienThi::thietLapLuongCay(int hang, int cot, HeThongVuon *heThong)
{
    if (hang >= 0 && hang < soHang && cot >= 0 && cot < soCot)
    {
        // XÃ³a há»‡ thá»‘ng cÅ© náº¿u cÃ³
        if (luongCay[hang][cot] != nullptr)
        {
            delete luongCay[hang][cot];
        }

        // GÃ¡n há»‡ thá»‘ng má»›i
        luongCay[hang][cot] = heThong;
    }
}

// Cáº¬P NHáº¬T HIá»‚N THá»Š - HÃ€M CHÃNH Cáº¬P NHáº¬T
void HienThi::capNhatHienThi()
{
    capNhatTrangThaiNut();

    if (cheDoHienTai == CHE_DO_AUTO)
    {
        xuLyLogicTuDong();
    }

    static unsigned long lanCapNhatCuoi = 0;
    unsigned long thoiGianHienTai = millis();

    if (thoiGianHienTai - lanCapNhatCuoi > THOI_GIAN_CAP_NHAT_MAN_HINH)
    {
        lanCapNhatCuoi = thoiGianHienTai;

        bool canVeLai = false;

        for (int i = 0; i < soHang; i++)
        {
            for (int j = 0; j < soCot; j++)
            {
                if (luongCay[i][j] != nullptr)
                {
                    float doAmDat = luongCay[i][j]->layDoAmDat();
                    float nhietDo = luongCay[i][j]->layNhietDo();
                    float anhSang = luongCay[i][j]->layAnhSang();

                    luongCay[i][j]->capNhatDuLieuCamBien(doAmDat, nhietDo, anhSang);

                    if (cheDoHienTai == CHE_DO_AUTO)
                    {
                        luongCay[i][j]->capNhat();
                    }

                    if (i == luongDangChon[0] && j == luongDangChon[1])
                    {
                        canVeLai = true;
                    }
                }
            }
        }

        if (trangThaiHienThi == 0 && canVeLai)
        {
            veGiaoDien();
        }
    }
}

// PHÆ¯Æ NG THá»¨C Láº¤Y Há»† THá»NG VÆ¯á»œN Táº I Vá»Š TRÃ
HeThongVuon *HienThi::layHeThongVuon(int hang, int cot)
{
    if (hang >= 0 && hang < soHang && cot >= 0 && cot < soCot)
    {
        return luongCay[hang][cot];
    }
    return nullptr;
}

// PHÆ¯Æ NG THá»¨C XÃ“A CÃ‚Y Táº I Vá»Š TRÃ
void HienThi::xoaCayTaiViTri(int hang, int cot)
{
    if (hang >= 0 && hang < soHang && cot >= 0 && cot < soCot)
    {
        if (luongCay[hang][cot] != nullptr)
        {
            delete luongCay[hang][cot];
            luongCay[hang][cot] = nullptr;

            String thongBao = "Da xoa cay tai: [";
            thongBao += hang;
            thongBao += "][";
            thongBao += cot;
            thongBao += "]";
            thongBaoCanhBao(thongBao);
        }
    }
}

// PHÆ¯Æ NG THá»¨C KHá»žI Táº O Láº I Há»† THá»NG
void HienThi::khoiTaoLaiHeThong()
{
    // XÃ³a táº¥t cáº£ cÃ¢y hiá»‡n cÃ³
    for (int i = 0; i < soHang; i++)
    {
        for (int j = 0; j < soCot; j++)
        {
            if (luongCay[i][j] != nullptr)
            {
                delete luongCay[i][j];
                luongCay[i][j] = nullptr;
            }
        }
    }

    // Reset vá» tráº¡ng thÃ¡i ban Ä‘áº§u
    trangThaiHienThi = 0;
    cheDoHienTai = CHE_DO_AUTO;
    luongDangChon[0] = 0;
    luongDangChon[1] = 0;

    // Váº½ láº¡i giao diá»‡n
    veGiaoDien();

    thongBaoCanhBao("He thong da duoc tao lai!");
}
void HienThi::ganFirebase(FirebaseData *fbdo, const String &id)
{
    fbdoPtr = fbdo;
    gardenId = id;
}
