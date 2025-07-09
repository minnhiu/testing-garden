// ignore: file_names
class Plant {
  final int id;
  final String name;
  final String imageUrl;
  final String description;
  final String temperatureAdvice;
  final String soilMoistureAdvice;
  final String wateringCycleAdvice;
  final String lightAdvice;
  int nguongDoAmDat = 100;
  int nguongAnhSang = 800;
  int nguongNhietDo = 40;
  final String potId;
  


  Plant({
    required this.id,
    required this.name,
    required this.imageUrl,
    required this.description,
    required this.temperatureAdvice,
    required this.soilMoistureAdvice,
    required this.wateringCycleAdvice,
    required this.lightAdvice,
    required this.nguongDoAmDat,
    required this.potId,
    required this.nguongAnhSang,
    required this.nguongNhietDo,
  });
}

List<Plant> plants = [
   Plant(
    id: 1,
    name: "Cây Cà Chua",
    imageUrl: "lib/images/cay-ca-chua.jpg",
    description: "Cây Cà Chua là loại rau ăn quả phổ biến, dễ trồng, thường được trồng tại vườn nhà hoặc trong chậu. Không chỉ cung cấp thực phẩm giàu vitamin A, C, mà còn giúp thanh lọc cơ thể và hỗ trợ hệ miễn dịch.",
    temperatureAdvice: "Nhiệt độ lý tưởng: 20-30°C.",
    soilMoistureAdvice: "Giữ ẩm đều, không úng.",
    wateringCycleAdvice: "Tưới 1-2 lần/ngày.",
    lightAdvice: "Cần nhiều ánh sáng trực tiếp.",
    potId: "gardenId1",
    nguongDoAmDat: 40,
    nguongAnhSang: 600,
    nguongNhietDo: 27,
  ),
  Plant(
    id: 2,
    name: "Cây Rau Xà Lách",
    imageUrl: "lib/images/rau-xa-lach.jpg",
    description: "Rau Xà Lách là loại rau lá được ưa chuộng trong các món salad, dễ trồng, phát triển nhanh, phù hợp với vùng khí hậu mát và đất tơi xốp.",
    temperatureAdvice: "Nhiệt độ lý tưởng: 15-22°C.",
    soilMoistureAdvice: "Giữ đất ẩm liên tục.",
    wateringCycleAdvice: "Tưới mỗi ngày 1 lần.",
    lightAdvice: "Thích hợp ánh sáng dịu, tránh nắng gắt.",
    potId: "gardenId2",
    nguongDoAmDat: 50,
    nguongAnhSang: 450,
    nguongNhietDo: 18,
  ),
  Plant(
    id: 3,
    name: "Cây Húng Quế",
    imageUrl: "lib/images/hung-que.jpg",
    description: "Húng Quế là loại rau thơm có mùi vị đặc trưng, thường dùng trong các món ăn Việt. Cây dễ trồng, thích ánh sáng và khí hậu ấm áp.",
    temperatureAdvice: "Nhiệt độ lý tưởng: 25-30°C.",
    soilMoistureAdvice: "Giữ ẩm nhẹ, không ngập úng.",
    wateringCycleAdvice: "Tưới 1 lần/ngày.",
    lightAdvice: "Cần ánh sáng mặt trời trực tiếp.",
    potId: "gardenId3",
    nguongDoAmDat: 35,
    nguongAnhSang: 550,
    nguongNhietDo: 25,
  ),
  Plant(
    id: 4,
    name: "Cây Bạc Hà",
    imageUrl: "lib/images/bac-ha.jpg",
    description: "Cây Bạc Hà có hương thơm dễ chịu, thường được trồng làm rau thơm và làm thuốc. Cây phát triển tốt trong điều kiện mát mẻ và ẩm.",
    temperatureAdvice: "Nhiệt độ lý tưởng: 20-26°C.",
    soilMoistureAdvice: "Ưa đất ẩm nhưng thoát nước tốt.",
    wateringCycleAdvice: "Tưới 1-2 lần/ngày tùy thời tiết.",
    lightAdvice: "Thích hợp nắng nhẹ hoặc nửa bóng râm.",
    potId: "gardenId4",
    nguongDoAmDat: 45,
    nguongAnhSang: 400,
    nguongNhietDo: 25,
  ),
  Plant(
    id: 5,
    name: "Cây Dưa Chuột",
    imageUrl: "lib/images/dua-chuot.jpg",
    description: "Dưa Chuột là loại cây leo dễ trồng, phát triển nhanh, quả mọng nước, thích hợp khí hậu nhiệt đới.",
    temperatureAdvice: "Nhiệt độ lý tưởng: 24-30°C.",
    soilMoistureAdvice: "Cần đất ẩm sâu và thoát nước.",
    wateringCycleAdvice: "Tưới 2 lần/ngày vào sáng và chiều.",
    lightAdvice: "Yêu cầu ánh sáng mặt trời mạnh.",
    potId: "gardenId5",
    nguongDoAmDat: 55,
    nguongAnhSang: 650,
    nguongNhietDo: 25,
  ),
  Plant(
    id: 6,
    name: "Cây Dâu",
    imageUrl: "lib/images/cay-dau.jpg",
    description: "Cây Dâu (Dâu tây) là cây ăn quả nhỏ, mọc thành cụm, ưa khí hậu mát. Cần chăm sóc kỹ nhưng cho trái ngọt, giàu vitamin C.",
    temperatureAdvice: "Nhiệt độ lý tưởng: 18-24°C.",
    soilMoistureAdvice: "Ưa đất ẩm, không để khô hạn.",
    wateringCycleAdvice: "Tưới sáng - chiều mỗi ngày.",
    lightAdvice: "Cần ánh sáng đầy đủ nhưng không nắng gắt.",
    potId: "gardenId6",
    nguongDoAmDat: 45,
    nguongAnhSang: 550,
    nguongNhietDo: 24,
  ),
  Plant(
    id: 7,
    name: "Cây Tùy Chỉnh",
    imageUrl: "lib/images/plant-placeholder.jpg",
    description: "Cấu hình cây được tuỳ chỉnh theo người dùng. Hãy đặt thông số phù hợp với điều kiện chăm sóc của bạn.",
    temperatureAdvice: "Tuỳ chỉnh.",
    soilMoistureAdvice: "Tuỳ chỉnh.",
    wateringCycleAdvice: "Tuỳ chỉnh.",
    lightAdvice: "Tuỳ chỉnh.",
    potId: "gardenId7",
    nguongDoAmDat: 30,
    nguongAnhSang: 500,
    nguongNhietDo: 28,
  ),
];
