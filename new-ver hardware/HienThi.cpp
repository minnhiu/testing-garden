#include "HienThi.h"
#include <stdint.h>
#include <Firebase_ESP_Client.h>

private:
FirebaseData *fbdoPtr = nullptr; // con trỏ tới FirebaseData

public:
void ganFirebase(FirebaseData *fbdo); // khai báo hàm gán fbdo
const unsigned long THOI_GIAN_CAP_NHAT_MAN_HINH = 8000;
#define TFT_DARKRED tft.color565(139, 0, 0)
#define TFT_GRAY tft.color565(128, 128, 128)
#define TFT_LIGHTBLUE tft.color565(173, 216, 230)
bool daChuyenSangChonCay = false;

// Constructor - Cải tiến layout và kích thước nút
HienThi::HienThi(int h, int c) : soHang(h),
                                 soCot(c),
                                 trangThaiHienThi(0),
                                 cheDoHienTai(CHE_DO_AUTO),
                                 daCham(false),
                                 diemChamX(0),
                                 diemChamY(0)
{
    // Khởi tạo màn hình TFT
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(MAU_NEN);

    // Khởi tạo vector luongCay với kích thước soHang x soCot
    luongCay.resize(soHang);
    for (int i = 0; i < soHang; i++)
    {
        luongCay[i].resize(soCot, nullptr);
    }

    // Thiết lập luống đang chọn mặc định
    luongDangChon[0] = 0;
    luongDangChon[1] = 0;

    // KHỞI TẠO CÁC NÚT VỚI KÍCH THƯỚC VÀ VỊ TRÍ

    // Nút Menu (góc phải trên)
    nutMenu = {230, 210, 80, 25, "MENU", false};

    // Nút Quay lại cho màn hình chính (góc trái dưới)
    nutQuayLai = {10, 210, 80, 25, "<THOAT", false};
    // Nút Thoát riêng cho giao diện MENU
    nutThoatMenu = {(tft.width() - 80) / 2, 200, 80, 25, "<THOAT", false};

    // Nút chuyển luống
    nutChonCay = {120, 210, 80, 25, "NEXT>", false};

    // Các nút điều khiển mới với spacing hợp lý
    nutAutoManual = {180, 120, 120, 35, "MANUAL", false};
    nutCaiDat = {50, 180, 100, 35, "CAI DAT", false};
    nutThongKe = {170, 180, 100, 35, "THONG KE", false};

    // Các nút điều khiển thiết bị (cho chế độ manual) - căn đều
    nutBom = {40, 180, 80, 25, "BOM", false};
    nutQuat = {130, 180, 80, 25, "QUAT", false};
    nutDen = {220, 180, 80, 25, "DEN", false};

    // Khởi tạo các nút chọn loại cây với layout grid 2x3
    nutChonLoaiCay.resize(SO_LOAI_CAY);
    int startX = 40;
    int startY = 70;
    int buttonWidth = 100;
    int buttonHeight = 30;
    int spacingX = 120;
    int spacingY = 40;

    // Sắp xếp theo grid 2 cột
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

    // Thiết lập cảm ứng
    uint16_t calData[5] = {275, 3620, 264, 3532, 1};
    tft.setTouch(calData);
}

// VẼ HEADER VỚI GRADIENT ĐẸP HƠN
void HienThi::veHeader(const String &tieuDe, uint16_t mauChinh)
{
    // Vẽ gradient header với hiệu ứng đẹp hơn
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

    // Đường viền header
    tft.drawFastHLine(0, 35, 320, TFT_WHITE);

    // Tiêu đề căn giữa
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    int textWidth = tieuDe.length() * 12;
    int x = (320 - textWidth) / 2;
    tft.setCursor(x, 14);
    tft.println(tieuDe);
}

// Gán thông tin phần cứng
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

// VẼ CARD THÔNG TIN VỚI SHADOW VÀ BORDER ĐẸP
void HienThi::veCardThongTin(int x, int y, int w, int h)
{
    // Hiệu ứng bóng
    tft.fillRoundRect(x + 2, y + 2, w, h, 8, TFT_DARKGREY);

    // Card chính
    tft.fillRoundRect(x, y, w, h, 8, TFT_BLACK);

    // Viền với gradient
    tft.drawRoundRect(x, y, w, h, 8, TFT_CYAN);
    tft.drawRoundRect(x + 1, y + 1, w - 2, h - 2, 7, TFT_LIGHTBLUE);
}

// CẢI TIẾN HÀM VẼ THANH TIẾN TRÌNH
void HienThi::veThanhTienTrinh(int x, int y, int width, int height, float giaTriHienTai, float giaTriToiDa, uint16_t mauThanh)
{
    // Viền
    tft.drawRoundRect(x, y, width, height, 3, TFT_WHITE);
    tft.fillRoundRect(x + 1, y + 1, width - 2, height - 2, 2, TFT_BLACK);

    // Tính toán chiều rộng thanh
    int chieuRongThanh = (int)((giaTriHienTai / giaTriToiDa) * (width - 4));
    if (chieuRongThanh > width - 4)
        chieuRongThanh = width - 4;
    if (chieuRongThanh < 0)
        chieuRongThanh = 0;

    // Vẽ thanh với gradient màu theo mức độ
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

// VẼ GIAO DIỆN CHÍNH
void HienThi::veGiaoDienChinh()
{
    tft.fillScreen(TFT_BLACK);

    // Reset lại vị trí nút
    nutQuayLai = {10, 210, 80, 25, "<THOAT", false};
    nutChonCay = {120, 210, 80, 25, "NEXT>", false};
    nutMenu = {230, 210, 80, 25, "MENU", false};

    nutBom.y = 180;
    nutQuat.y = 180;
    nutDen.y = 180;

    // Tiêu đề
    veHeader("VƯỜN THÔNG MINH", TFT_CYAN);

    // Thông tin chế độ
    tft.fillRect(10, 40, 200, 20, TFT_BLACK); // Xóa nền dòng trước
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 40);
    tft.print("Chế độ: ");
    tft.setTextColor(cheDoHienTai == CHE_DO_AUTO ? TFT_GREEN : TFT_ORANGE);
    tft.print(cheDoHienTai == CHE_DO_AUTO ? "TỰ ĐỘNG" : "THỦ CÔNG");

    // Vị trí luống đang chọn
    int hang, cot;
    layLuongDangChon(hang, cot);
    HeThongVuon *cay = luongCay[hang][cot];

    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(2);
    tft.setCursor(10, 60);
    tft.print("LUỐNG [" + String(hang) + "][" + String(cot) + "]");

    if (cay == nullptr)
    {
        // Nếu chưa trồng cây
        tft.setTextColor(TFT_GREEN);
        tft.setCursor(10, 90);
        tft.print("CHƯA CÓ CÂY TẠI LUỐNG NÀY");

        veNut(nutMenu);
        veNut(nutChonCay);
        veNut(nutQuayLai);
        return;
    }

    // Loại cây
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(10, 80);
    tft.print("Loại: ");
    tft.print(cay->layCauHinhCay().layTen());

    // Hiển thị cảm biến
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

    // Nút điều khiển thiết bị (nếu ở chế độ thủ công)
    if (cheDoHienTai == CHE_DO_MANUAL)
    {
        veNut(nutBom);
        veNut(nutQuat);
        veNut(nutDen);
    }

    // Nút điều hướng chung
    veNut(nutMenu);
    veNut(nutChonCay);
    veNut(nutQuayLai);
}

// CẢI TIẾN GIAO DIỆN MENU
void HienThi::veGiaoDienMenu()
{
    tft.fillScreen(TFT_BLACK);
    veHeader("MENU CHÍNH", TFT_GREEN);

    tft.setTextSize(2);
    tft.setTextColor(TFT_ORANGE);
    tft.setCursor(10, 40);
    tft.print("Chế độ: ");
    tft.print(cheDoHienTai == CHE_DO_AUTO ? "TỰ ĐỘNG" : "THỦ CÔNG");

    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 60);
    tft.print("Chọn chức năng:");

    // Đặt vị trí đều 5 nút
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

    // Vẽ nút
    veNut(nutAutoManual);
    veNut(nutThongKe);
    veNut(nutTrongCay);
    veNut(nutCaiDat);
    veNut(nutQuayLai);
}

// GIAO DIỆN CHỌN LOẠI CÂY
void HienThi::veGiaoDienChonLoaiCay()
{
    tft.fillScreen(TFT_BLACK);

    veHeader("CHỌN LOẠI CÂY", TFT_GREEN);

    // Hiển thị vị trí đang chọn
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(2);
    tft.setCursor(20, 45);
    tft.print("Luống [");
    tft.print(luongDangChon[0]);
    tft.print("][");
    tft.print(luongDangChon[1]);
    tft.print("]");

    // Nếu đã trồng cây thì hiển thị tên
    HeThongVuon *cay = layHeThongVuon(luongDangChon[0], luongDangChon[1]);
    if (cay != nullptr)
    {
        tft.setTextColor(TFT_CYAN);
        tft.setCursor(20, 70); // hiển thị tại dòng riêng
        tft.print("Đã có: ");
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

    // Nút quay lại
    nutQuayLai = {10, 210, 80, 25, "THOAT", false};
    veNut(nutQuayLai);
}

// HÀM CHÍNH VẼ GIAO DIỆN
void HienThi::veGiaoDien()
{
    switch (trangThaiHienThi)
    {
    case 0: // Giao diện chính
        veGiaoDienChinh();
        break;

    case 1: // Menu
        veGiaoDienMenu();
        break;

    case 2: // Điều khiển
        veManHinhDieuKhien();
        break;

    case 3: // Chọn loại cây
        veGiaoDienChonLoaiCay();
        break;

    case 4: // Cài đặt
        tft.fillScreen(TFT_BLACK);
        veHeader("CÀI ĐẶT", TFT_YELLOW);

        tft.setTextColor(TFT_CYAN);
        tft.setTextSize(2);
        tft.setCursor(20, 60);
        tft.println("Các tùy chọn cài đặt:");

        tft.setTextColor(TFT_WHITE);
        tft.setCursor(20, 80);
        tft.println("- Ngưỡng cảm biến");
        tft.setCursor(20, 95);
        tft.println("- Thời gian tưới nước");
        tft.setCursor(20, 125);
        tft.println("- Reset hệ thống");

        tft.setTextColor(TFT_YELLOW);
        tft.setCursor(20, 150);
        tft.println("Tính năng đang phát triển...");

        veNut(nutQuayLai);
        break;

    case 5: // Thống kê
        tft.fillScreen(TFT_BLACK);
        veHeader("THỐNG KÊ", TFT_RED);

        // Tính toán thống kê
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
        tft.println("Thống kê tổng quan:");

        tft.setTextColor(TFT_WHITE);
        tft.setCursor(20, 80);
        tft.print("Số cây đã trồng: ");
        tft.setTextColor(TFT_GREEN);
        tft.print(soCayDaTrong);
        tft.setTextColor(TFT_WHITE);
        tft.print("/");
        tft.print(soHang * soCot);

        tft.setCursor(20, 100);
        tft.print("Số cây cần tưới: ");
        tft.setTextColor(soCayCanTuoi > 0 ? TFT_YELLOW : TFT_GREEN);
        tft.print(soCayCanTuoi);

        tft.setTextColor(TFT_WHITE);
        tft.setCursor(20, 120);
        tft.print("Số bơm đang bật: ");
        tft.setTextColor(TFT_CYAN);
        tft.print(soBomDangBat);

        // Hiệu suất hệ thống
        float hieuSuat = (float)soCayDaTrong / (soHang * soCot) * 100;
        tft.setTextColor(TFT_YELLOW);
        tft.setCursor(20, 150);
        tft.println("Hiệu suất hệ thống:");

        tft.setTextColor(TFT_WHITE);
        tft.setCursor(20, 170);
        tft.print("Sử dụng: ");
        tft.setTextColor(hieuSuat > 70 ? TFT_GREEN : (hieuSuat > 40 ? TFT_YELLOW : TFT_RED));
        tft.print(hieuSuat, 1);
        tft.print("%");

        veThanhTienTrinh(20, 190, 200, 15, hieuSuat, 100, TFT_GREEN);

        veNut(nutQuayLai);
        break;
    }
}

// CẬP NHẬT XỬ LÝ TOUCH ĐỂ PHẢN HỒI VỚI LAYOUT MỚI
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

        daCham = false; // Reset để cho phép chạm lại
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
    case 0: // Màn hình chính
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
            thongBaoCanhBao("Hệ thống đang hoạt động!");
        }
        else if (cheDoHienTai == CHE_DO_MANUAL)
        {
            xuLyDieuKhienThuCong(x, y); // Xử lý các nút BƠM / QUẠT / ĐÈN tại màn chính
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
                            // Nếu vừa chuyển sang thủ công thì tắt hết relay
                            luongCay[i][j]->datTrangThaiBom(false);
                            luongCay[i][j]->datTrangThaiDen(false);
                            luongCay[i][j]->datTrangThaiQuat(false);
                        }
                    }
                }
            }
            thongBaoCanhBao(cheDoHienTai == CHE_DO_AUTO ? "Chế độ TỰ ĐỘNG" : "Chế độ THỦ CÔNG");
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
            // Sau này thêm giao diện cài đặt
            thongBaoCanhBao("Đang phát triển");
        }
        else if (kiemTraNut(nutQuayLai, x, y))
        {
            trangThaiHienThi = 0;
            veGiaoDien();
        }
        break;

    case 2: // Màn hình điều khiển
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

    case 3: // Giao diện chọn loại cây
        if (kiemTraNut(nutQuayLai, x, y))
        {
            trangThaiHienThi = 1; // Về MENU
            veGiaoDien();
        }
        else
        {
            for (int i = 0; i < SO_LOAI_CAY; i++)
            {
                if (kiemTraNut(nutChonLoaiCay[i], x, y))
                {
                    // Trồng cây tại vị trí đang chọn
                    trongCayTaiViTri(luongDangChon[0], luongDangChon[1], i);

                    // Đảm bảo luongDangChon vẫn trỏ đúng vị trí vừa trồng
                    // (Đã được cập nhật trong hàm trongCayTaiViTri)

                    trangThaiHienThi = 0; // Về giao diện chính
                    veGiaoDien();
                    break;
                }
            }
        }
        break;

    case 4: // Cài đặt
    case 5: // Thống kê
        if (kiemTraNut(nutQuayLai, x, y))
        {
            trangThaiHienThi = 1;
            veGiaoDien();
        }
        break;
    }
}

// XỬ LÝ ĐIỀU KHIỂN THỦ CÔNG
void HienThi::xuLyDieuKhienThuCong(int x, int y)
{
    int hang = luongDangChon[0];
    int cot = luongDangChon[1];

    if (luongCay[hang][cot] == nullptr)
    {
        thongBaoCanhBao("Không có cây tại vị trí này!");
        return;
    }

    bool coThayDoi = false;

    if (kiemTraNut(nutBom, x, y))
    {
        bool trangThaiMoi = !luongCay[hang][cot]->layTrangThaiBom();
        luongCay[hang][cot]->datTrangThaiBom(trangThaiMoi);
        thongBaoCanhBao(trangThaiMoi ? "Bật bơm" : "Tắt bơm");
        coThayDoi = true;

        // Gửi Firebase
        if (fbdoPtr != nullptr && Firebase.ready())
        {
            String gardenId = "gardenId" + String(hang * SO_COT + cot + 1);
            Firebase.RTDB.setInt(fbdoPtr, gardenId + "/mayBom", trangThaiMoi ? 1 : 0);
        }
    }
    else if (kiemTraNut(nutQuat, x, y))
    {
        bool trangThaiMoi = !luongCay[hang][cot]->layTrangThaiQuat();
        luongCay[hang][cot]->datTrangThaiQuat(trangThaiMoi);
        thongBaoCanhBao(trangThaiMoi ? "Bật quạt" : "Tắt quạt");
        coThayDoi = true;

        if (fbdoPtr != nullptr && Firebase.ready())
        {
            String gardenId = "gardenId" + String(hang * SO_COT + cot + 1);
            Firebase.RTDB.setInt(fbdoPtr, gardenId + "/quat", trangThaiMoi ? 1 : 0);
        }
    }
    else if (kiemTraNut(nutDen, x, y))
    {
        bool trangThaiMoi = !luongCay[hang][cot]->layTrangThaiDen();
        luongCay[hang][cot]->datTrangThaiDen(trangThaiMoi);
        thongBaoCanhBao(trangThaiMoi ? "Bật đèn" : "Tắt đèn");
        coThayDoi = true;

        if (fbdoPtr != nullptr && Firebase.ready())
        {
            String gardenId = "gardenId" + String(hang * SO_COT + cot + 1);
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
        thongBaoCanhBao("Chuyển luống");
        coThayDoi = true;
    }

    if (coThayDoi)
    {
        veGiaoDien(); // Quay lại đúng giao diện hiện tại
    }
}

// MÀN HÌNH ĐIỀU KHIỂN THỦ CÔNG
void HienThi::veManHinhDieuKhien()
{
    tft.fillScreen(TFT_BLACK);

    veHeader("ĐIỀU KHIỂN THỦ CÔNG", TFT_RED);

    int hang = luongDangChon[0];
    int cot = luongDangChon[1];

    // Hiển thị thông tin luống đang chọn
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(1);
    tft.setCursor(20, 45);
    tft.print("Luống [");
    tft.print(hang);
    tft.print("][");
    tft.print(cot);
    tft.print("]");

    if (luongCay[hang][cot] != nullptr)
    {
        tft.setTextColor(TFT_CYAN);
        tft.print(" - ");
        tft.print(luongCay[hang][cot]->layCauHinhCay().layTen());

        // Card thông tin cảm biến
        veCardThongTin(10, 65, 300, 120);

        int yPos = 75;
        int spacing = 25;

        // Hiển thị thông tin cảm biến
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);

        // Độ ẩm đất
        veBieuTuongDoAm(20, yPos);
        tft.setCursor(40, yPos);
        tft.print("Độ ẩm: ");
        tft.setTextColor(TFT_CYAN);
        tft.print(luongCay[hang][cot]->layDoAmDat(), 1);
        tft.print("%");

        // Nhiệt độ
        yPos += spacing;
        veBieuTuongNhietDo(20, yPos);
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(40, yPos);
        tft.print("Nhiệt độ: ");
        tft.setTextColor(TFT_RED);
        tft.print(luongCay[hang][cot]->layNhietDo(), 1);
        tft.print("C");

        // Ánh sáng
        yPos += spacing;
        veBieuTuongAnhSang(20, yPos);
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(40, yPos);
        tft.print("Ánh sáng: ");
        tft.setTextColor(TFT_YELLOW);
        tft.print(luongCay[hang][cot]->layAnhSang(), 1);
        tft.print(" lux");

        // Trạng thái thiết bị hiện tại
        yPos += spacing;
        tft.setTextColor(TFT_LIGHTBLUE);
        tft.setCursor(20, yPos);
        tft.print("Trạng thái hiện tại:");

        yPos += 15;
        int xPos = 20;

        // Hiển thị trạng thái bơm
        tft.fillRoundRect(xPos, yPos, 60, 15, 3,
                          luongCay[hang][cot]->layTrangThaiBom() ? TFT_GREEN : TFT_RED);
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(xPos + 5, yPos + 3);
        tft.print("BƠM: ");
        tft.print(luongCay[hang][cot]->layTrangThaiBom() ? "ON" : "OFF");

        xPos += 70;
        // Hiển thị trạng thái quạt
        tft.fillRoundRect(xPos, yPos, 60, 15, 3,
                          luongCay[hang][cot]->layTrangThaiQuat() ? TFT_GREEN : TFT_RED);
        tft.setCursor(xPos + 5, yPos + 3);
        tft.print("QUẠT: ");
        tft.print(luongCay[hang][cot]->layTrangThaiQuat() ? "ON" : "OFF");

        xPos += 70;
        // Hiển thị trạng thái đèn
        tft.fillRoundRect(xPos, yPos, 60, 15, 3,
                          luongCay[hang][cot]->layTrangThaiDen() ? TFT_GREEN : TFT_RED);
        tft.setCursor(xPos + 5, yPos + 3);
        tft.print("ĐÈN: ");
        tft.print(luongCay[hang][cot]->layTrangThaiDen() ? "ON" : "OFF");
    }
    else
    {
        tft.setTextColor(TFT_GRAY);
        tft.setTextSize(2);
        tft.setCursor(80, 120);
        tft.println("KHÔNG CÓ CÂY");
    }

    // Các nút điều khiển
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(20, 200);
    tft.println("Điều khiển thiết bị:");

    // Vẽ các nút điều khiển với vị trí mới
    nutBom.y = 220;
    nutQuat.y = 220;
    nutDen.y = 220;
    nutChonCay.y = 260;

    veNut(nutBom);
    veNut(nutQuat);
    veNut(nutDen);
    veNut(nutChonCay);
    veNut(nutQuayLai);

    // Hướng dẫn sử dụng
    tft.setTextColor(TFT_LIGHTBLUE);
    tft.setTextSize(2);
    tft.setCursor(10, 290);
    tft.println("Chạm nút để bật/tắt thiết bị");
}

// VẼ CÁC BIỂU TƯỢNG CẢM BIẾN
void HienThi::veBieuTuongDoAm(int x, int y)
{
    // Vẽ biểu tượng giọt nước
    tft.fillCircle(x + 8, y + 10, 5, TFT_BLUE);
    tft.fillTriangle(x + 8, y + 2, x + 3, y + 10, x + 13, y + 10, TFT_BLUE);
}

void HienThi::veBieuTuongNhietDo(int x, int y)
{
    // Vẽ biểu tượng nhiệt kế
    tft.drawRoundRect(x + 4, y, 6, 14, 3, TFT_RED);
    tft.fillCircle(x + 7, y + 16, 3, TFT_RED);
    tft.drawFastVLine(x + 7, y + 2, 10, TFT_RED);
}

void HienThi::veBieuTuongAnhSang(int x, int y)
{
    // Vẽ biểu tượng mặt trời
    tft.fillCircle(x + 8, y + 8, 5, TFT_YELLOW);
    // Các tia sáng
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

// VẼ NÚT VỚI HIỆU ỨNG
void HienThi::veNut(ThongTinNut nut)
{
    // Vẽ nền và viền nút
    tft.fillRoundRect(nut.x, nut.y, nut.chieuRong, nut.chieuCao, 5, TFT_CYAN);
    tft.drawRoundRect(nut.x, nut.y, nut.chieuRong, nut.chieuCao, 5, TFT_BLUE);

    // Giảm kích thước chữ để tránh tràn
    tft.setTextSize(1); // Nhỏ hơn, gọn hơn
    tft.setTextColor(TFT_BLACK);

    // Tính chiều rộng/chiều cao chữ
    int charWidth = 6;  // Mỗi ký tự ~6px ở size 1
    int charHeight = 8; // Chiều cao ở size 1
    int textWidth = nut.nhan.length() * charWidth;

    // Căn giữa chữ
    int textX = nut.x + (nut.chieuRong - textWidth) / 2;
    int textY = nut.y + (nut.chieuCao - charHeight) / 2;

    tft.setCursor(textX, textY);
    tft.print(nut.nhan);
}

// KIỂM TRA NÚT ĐƯỢC NHẤN
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

// THÔNG BÁO CẢNH BÁO
void HienThi::thongBaoCanhBao(const String &thongDiep)
{
    int popupW = 200;
    int popupH = 60;
    int popupX = (tft.width() - popupW) / 2;
    int popupY = 80;
    int maxPixel = popupW - 20; // Padding trái/phải

    // Shadow + nền
    tft.fillRoundRect(popupX + 3, popupY + 3, popupW, popupH, 8, TFT_DARKGREY);
    tft.fillRoundRect(popupX, popupY, popupW, popupH, 8, TFT_ORANGE);
    tft.drawRoundRect(popupX, popupY, popupW, popupH, 8, TFT_RED);

    tft.setTextSize(2);
    tft.setTextColor(TFT_BLACK);

    // Tách chuỗi thành từ
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

    // Gộp từ vào dòng 1
    for (size_t i = 0; i < tu.size(); i++)
    {
        String word = tu[i];
        int w = word.length() * 12 + (dong1 == "" ? 0 : 12); // Khoảng trắng

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
            dong2 += "..."; // Nếu quá dài thì rút gọn
            break;
        }
    }

    // Hiển thị dòng 1
    int textX1 = popupX + (popupW - dong1.length() * 12) / 2;
    tft.setCursor(textX1, popupY + 15);
    tft.print(dong1);

    // Hiển thị dòng 2 nếu có
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

    String thongBao = "Chuyển sang chế độ ";
    thongBao += (cheDoHienTai == CHE_DO_AUTO) ? "TỰ ĐỘNG" : "THỦ CÔNG";
    thongBaoCanhBao(thongBao);

    // Gửi currentMode lên Firebase nếu đã có fbdo
    if (fbdoPtr != nullptr && Firebase.ready())
    {
        String modeStr = (cheDoHienTai == CHE_DO_AUTO) ? "auto" : "manual";
        if (!Firebase.RTDB.setString(fbdoPtr, "currentMode", modeStr))
        {
            Serial.print("Lỗi gửi chế độ lên Firebase: ");
            Serial.println(fbdoPtr->errorReason());
        }
        else
        {
            Serial.print("Đã cập nhật chế độ lên Firebase: ");
            Serial.println(modeStr);
        }
    }
}

// TRỒNG CÂY TẠI VỊ TRÍ
void HienThi::trongCayTaiViTri(int hang, int cot, int loaiCay)
{
    if (hang >= 0 && hang < soHang && cot >= 0 && cot < soCot)
    {
        // Xóa cây cũ nếu có
        if (luongCay[hang][cot] != nullptr)
        {
            delete luongCay[hang][cot];
        }

        // Tạo cảm biến cho vị trí tương ứng
        CamBien camDoAm(chanDatAm[hang][cot], 1, 0, 100, 0, 4095);
        CamBien camAnhSang(chanAnhSang[hang][cot], 1, 0, 0, 10000, 4095);
        CamBien camNhiet(chanDHT[hang][cot], 2);

        // Tạo hệ thống vườn mới với cảm biến
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

        // Thiết lập loại cây
        CauHinhCay cauHinh = CauHinhCay::layCauHinhMacDinh(static_cast<LoaiCay>(loaiCay));
        cayMoi->thietLapCauHinhCay(cauHinh);

        // Gán hệ thống vào luống
        thietLapLuongCay(hang, cot, cayMoi);

        // Hiển thị thông báo
        String thongBao = "Trồng ";
        thongBao += nutChonLoaiCay[loaiCay].nhan;
        thongBao += " tại [" + String(hang) + "][" + String(cot) + "]";
        thongBaoCanhBao(thongBao);

        // Về giao diện chính
        trangThaiHienThi = 0;
        veGiaoDien();
    }
}

// CẬP NHẬT DỮ LIỆU TỪ CẢM BIẾN
void HienThi::capNhatDuLieuCamBien(int hang, int cot, float doAmDat, float nhietDo, float anhSang)
{
    if (hang >= 0 && hang < soHang && cot >= 0 && cot < soCot && luongCay[hang][cot] != nullptr)
    {
        luongCay[hang][cot]->capNhatDuLieuCamBien(doAmDat, nhietDo, anhSang);

        // Chỉ cập nhật giao diện nếu đang hiển thị luống này
        if (hang == luongDangChon[0] && cot == luongDangChon[1] && trangThaiHienThi == 0)
        {
            veGiaoDien();
        }
    }
}

// XỬ LÝ LOGIC TỰ ĐỘNG
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

// LẤY TRẠNG THÁI HỆ THỐNG
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

// LẤY THÔNG TIN LUỐNG ĐANG CHỌN
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

// THIẾT LẬP LUỐNG CÂY
void HienThi::thietLapLuongCay(int hang, int cot, HeThongVuon *heThong)
{
    if (hang >= 0 && hang < soHang && cot >= 0 && cot < soCot)
    {
        // Xóa hệ thống cũ nếu có
        if (luongCay[hang][cot] != nullptr)
        {
            delete luongCay[hang][cot];
        }

        // Gán hệ thống mới
        luongCay[hang][cot] = heThong;
    }
}

// CẬP NHẬT HIỂN THỊ - HÀM CHÍNH CẬP NHẬT
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

// PHƯƠNG THỨC LẤY HỆ THỐNG VƯỜN TẠI VỊ TRÍ
HeThongVuon *HienThi::layHeThongVuon(int hang, int cot)
{
    if (hang >= 0 && hang < soHang && cot >= 0 && cot < soCot)
    {
        return luongCay[hang][cot];
    }
    return nullptr;
}

// PHƯƠNG THỨC XÓA CÂY TẠI VỊ TRÍ
void HienThi::xoaCayTaiViTri(int hang, int cot)
{
    if (hang >= 0 && hang < soHang && cot >= 0 && cot < soCot)
    {
        if (luongCay[hang][cot] != nullptr)
        {
            delete luongCay[hang][cot];
            luongCay[hang][cot] = nullptr;

            String thongBao = "Đã xóa cây tại [";
            thongBao += hang;
            thongBao += "][";
            thongBao += cot;
            thongBao += "]";
            thongBaoCanhBao(thongBao);
        }
    }
}

// PHƯƠNG THỨC KHỞI TẠO LẠI HỆ THỐNG
void HienThi::khoiTaoLaiHeThong()
{
    // Xóa tất cả cây hiện có
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

    // Reset về trạng thái ban đầu
    trangThaiHienThi = 0;
    cheDoHienTai = CHE_DO_AUTO;
    luongDangChon[0] = 0;
    luongDangChon[1] = 0;

    // Vẽ lại giao diện
    veGiaoDien();

    thongBaoCanhBao("Hệ thống đã được khởi tạo lại!");
}