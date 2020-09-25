/*
 Navicat Premium Data Transfer

 Source Server         : localhost_3306
 Source Server Type    : MySQL
 Source Server Version : 80021
 Source Host           : localhost:3306
 Source Schema         : test

 Target Server Type    : MySQL
 Target Server Version : 80021
 File Encoding         : 65001

 Date: 24/09/2020 23:07:24
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for test_user
-- ----------------------------
DROP TABLE IF EXISTS `test_user`;
CREATE TABLE `test_user`  (
  `id` bigint UNSIGNED NOT NULL COMMENT 'id',
  `name` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL DEFAULT '' COMMENT 'name',
  `nickname` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL DEFAULT '' COMMENT 'nickname',
  `sex` tinyint UNSIGNED NOT NULL DEFAULT 0 COMMENT '1male,2women',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `uni_key`(`nickname`) USING BTREE,
  INDEX `account_id`(`name`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = COMPACT;

-- ----------------------------
-- Records of test_user
-- ----------------------------
INSERT INTO `test_user` VALUES (1, 'Bai Li', 'XiaoBai', 1);
INSERT INTO `test_user` VALUES (2, 'Lisi', 'xiaosi', 2);
INSERT INTO `test_user` VALUES (3, 'Wangwu', 'xiaowu', 1);
INSERT INTO `test_user` VALUES (4, 'Zhaoliu', 'xiaoliu', 2);
INSERT INTO `test_user` VALUES (5, 'Heiqi', 'xiaohei', 1);
INSERT INTO `test_user` VALUES (6, 'Wangba', 'dabai', 2);
INSERT INTO `test_user` VALUES (7, 'Zhaojiu', 'dajiu', 1);
INSERT INTO `test_user` VALUES (8, 'Hongshi', 'dashi', 2);

SET FOREIGN_KEY_CHECKS = 1;
