import 'dart:convert';
import 'package:intl/intl.dart';

class NewsApiResponse {
  final String status;
  final int totalArticles;
  final List<Article> articles;

  NewsApiResponse({
    required this.status,
    required this.totalArticles,
    required this.articles,
  });

  factory NewsApiResponse.fromRawJson(String str) =>
      NewsApiResponse.fromJson(json.decode(str));

  factory NewsApiResponse.fromJson(Map<String, dynamic> json) =>
      NewsApiResponse(
        status: json["status"] ?? "",
        totalArticles: json["totalArticles"] ?? 0,
        articles: json["articles"] == null
            ? []
            : List<Article>.from(
                json["articles"].map((x) => Article.fromJson(x))),
      );

  String toRawJson() => json.encode(toJson());

  Map<String, dynamic> toJson() => {
        "status": status,
        "totalArticles": totalArticles,
        "articles": List<dynamic>.from(articles.map((x) => x.toJson())),
      };
}

class Article {
  final String? title;
  final String? description;
  final String? content;
  final String? url;
  final String? image; // <-- Trường đúng từ GNews
  final DateTime? publishedAt;
  final Source? source;

  Article({
    this.title,
    this.description,
    this.content,
    this.url,
    this.image,
    this.publishedAt,
    this.source,
  });

  factory Article.fromRawJson(String str) => Article.fromJson(json.decode(str));

  factory Article.fromJson(Map<String, dynamic> json) => Article(
        title: json["title"],
        description: json["description"],
        content: json["content"],
        url: json["url"],
        image: json["image"],
        publishedAt: json["publishedAt"] == null
            ? null
            : DateTime.tryParse(json["publishedAt"]),
        source: json["source"] != null ? Source.fromJson(json["source"]) : null,
      );

  String toRawJson() => json.encode(toJson());

  Map<String, dynamic> toJson() => {
        "title": title,
        "description": description,
        "content": content,
        "url": url,
        "image": image,
        "publishedAt": publishedAt?.toIso8601String(),
        "source": source?.toJson(),
      };

  String getFormattedDate() {
    final formatter = DateFormat('dd MMM yyyy');
    return publishedAt != null
        ? formatter.format(publishedAt!)
        : 'Unknown date';
  }
}

class Source {
  final String? name;
  final String? url;

  Source({this.name, this.url});

  factory Source.fromRawJson(String str) => Source.fromJson(json.decode(str));

  factory Source.fromJson(Map<String, dynamic> json) => Source(
        name: json["name"],
        url: json["url"],
      );

  String toRawJson() => json.encode(toJson());

  Map<String, dynamic> toJson() => {
        "name": name,
        "url": url,
      };
}
