// import 'package:flutter/material.dart';
// import 'package:google_fonts/google_fonts.dart';
// import 'package:plantapp/pages/models/News.dart'; // Import Article model
// import 'package:url_launcher/url_launcher.dart'; // Import url_launcher

// class NewsCard extends StatelessWidget {
//   final Article article;

//   // Nhận vào bài viết Article để hiển thị
//   const NewsCard({super.key, required this.article});

//   _launchURL(String urlString) async {
//     final Uri uri = Uri.parse(urlString); // Sử dụng urlString được truyền vào
//     if (!await launchUrl(uri)) {
//       throw Exception('Could not launch $urlString');
//     }
//   }

//   @override
//   Widget build(BuildContext context) {
//     // Kiểm tra nếu URL hình ảnh không có giá trị, sử dụng một hình ảnh mặc định
//     String imageUrl = article.urlToImage ??
//         'https://via.placeholder.com/0'; // Hình ảnh mặc định nếu không có

//     return Card(
//       margin: const EdgeInsets.symmetric(vertical: 8, horizontal: 16),
//       elevation: 4,
//       child: InkWell(
//         onTap: () {
//           if (article.url != null && article.url!.isNotEmpty) {
//             _launchURL(article.url!); // Mở link trong trình duyệt
//           } else {
//             print('URL không hợp lệ hoặc rỗng.');
//           }
//         },
//         child: Column(
//           crossAxisAlignment: CrossAxisAlignment.start,
//           children: [
//             // Hình ảnh đại diện (nếu có)
//             ClipRRect(
//               borderRadius: BorderRadius.only(
//                 topLeft: Radius.circular(10),
//                 topRight: Radius.circular(10),
//               ),
//               child: Image.network(imageUrl,
//                   width: double.infinity, height: 180, fit: BoxFit.cover),
//             ),
//             Padding(
//               padding: const EdgeInsets.all(12.0),
//               child: Column(
//                 crossAxisAlignment: CrossAxisAlignment.start,
//                 children: [
//                   // Tiêu đề bài viết
//                   Text(
//                     article.title ?? 'No Title',
//                     style: GoogleFonts.poppins(
//                         fontSize: 18, fontWeight: FontWeight.bold),
//                   ),
//                   const SizedBox(height: 8),
//                   // Mô tả bài viết
//                   Text(
//                     article.description ?? 'No description available',
//                     style: GoogleFonts.poppins(
//                         fontSize: 14, color: Colors.black54),
//                   ),
//                   const SizedBox(height: 8),
//                   // Thời gian đăng bài
//                   Text(
//                     article.getDateOnly(),
//                     style: GoogleFonts.poppins(
//                         fontSize: 12, color: Colors.black54),
//                   ),
//                 ],
//               ),
//             ),
//           ],
//         ),
//       ),
//     );
//   }
// }

import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:plantapp/pages/models/News.dart';
import 'package:url_launcher/url_launcher.dart';

class NewsCard extends StatelessWidget {
  final Article article;

  const NewsCard({super.key, required this.article});

  _launchURL(String urlString) async {
    final Uri uri = Uri.parse(urlString);
    if (!await launchUrl(uri)) {
      throw Exception('Could not launch $urlString');
    }
  }

  bool _isValidImageUrl(String? url) {
    return url != null &&
        url.trim().isNotEmpty &&
        Uri.tryParse(url)?.hasAbsolutePath == true;
  }

  @override
  Widget build(BuildContext context) {
    final bool hasImage = _isValidImageUrl(article.urlToImage);

    return Card(
      margin: const EdgeInsets.symmetric(vertical: 8, horizontal: 16),
      elevation: 4,
      child: InkWell(
        onTap: () {
          if (article.url != null && article.url!.isNotEmpty) {
            _launchURL(article.url!);
          } else {
            print('URL không hợp lệ hoặc rỗng.');
          }
        },
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            // Nếu có hình ảnh hợp lệ thì mới hiển thị
            if (hasImage)
              ClipRRect(
                borderRadius: const BorderRadius.only(
                  topLeft: Radius.circular(10),
                  topRight: Radius.circular(10),
                ),
                child: Image.network(
                  article.urlToImage!,
                  width: double.infinity,
                  height: 180,
                  fit: BoxFit.cover,
                  errorBuilder: (context, error, stackTrace) {
                    return const SizedBox(); // Nếu lỗi khi tải ảnh, không hiển thị gì
                  },
                ),
              ),

            Padding(
              padding: const EdgeInsets.all(12.0),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    article.title ?? 'No Title',
                    style: GoogleFonts.poppins(
                      fontSize: 18,
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                  const SizedBox(height: 8),
                  Text(
                    article.description ?? 'No description available',
                    style: GoogleFonts.poppins(
                      fontSize: 14,
                      color: Colors.black54,
                    ),
                  ),
                  const SizedBox(height: 8),
                  Text(
                    article.getDateOnly(),
                    style: GoogleFonts.poppins(
                      fontSize: 12,
                      color: Colors.black54,
                    ),
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}
