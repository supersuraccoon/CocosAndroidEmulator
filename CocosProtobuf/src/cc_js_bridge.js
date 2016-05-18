//
test_protobuf = function (data) {
    cc.log("test_get_protobuf");
    cc.log("data: " + data);
    
    var ProtoBuf = dcodeIO.ProtoBuf;
    var GameInfoBuilder = ProtoBuf.newBuilder();
    
    // load .proto
    ProtoBuf.protoFromContents("res/GameEnum.proto", GameInfoBuilder, jsb.fileUtils.getStringFromFile("res/GameEnum.proto"));
    ProtoBuf.protoFromContents("res/GameInfo.proto", GameInfoBuilder, jsb.fileUtils.getStringFromFile("res/GameInfo.proto"));
    
    var Game = GameInfoBuilder.build("game");
    var gameInfo = Game.info.GameInfo.decode(data, "utf8");
    cc.log("\ndata decoded gameInfo: " + gameInfo);
    
    for (var i = 0; i < gameInfo.roleInfo.length; i ++) {
        var roleInfo = gameInfo.roleInfo[i];
        cc.log("roleInfo: \nname: " + roleInfo.name + " type: " + roleInfo.name + "\n");
    }
    for (var i = 0; i < gameInfo.itemInfo.length; i ++) {
        var itemInfo = gameInfo.itemInfo[i];
        cc.log("itemInfo: \nname: " + itemInfo.name + " price: " + itemInfo.price + "\n");
    }
    cc.log("\n------ ---------- ------\n\n");
    
    // send a GameInfo instance to c++
    var gameInfoOut = new Game.info.GameInfo();
    
    var roleInfo1 = new Game.info.RoleInfo();
    roleInfo1.name = "js_name1";
    roleInfo1.type = Game.enumeration.RoleType.FIGHTER;
    gameInfoOut.roleInfo.push(roleInfo1);
    
    var roleInfo2 = new Game.info.RoleInfo();
    roleInfo2.name = "js_name2";
    roleInfo2.type = Game.enumeration.RoleType.BOWMAN;
    gameInfoOut.roleInfo.push(roleInfo2);
    
    var itemInfo1 = new Game.info.ItemInfo();
    itemInfo1.name = "js_item1";
    itemInfo1.price = 100;
    gameInfoOut.itemInfo.push(itemInfo1);
    
    var itemInfo2 = new Game.info.ItemInfo();
    itemInfo2.name = "js_item2";
    itemInfo2.price = 200;
    gameInfoOut.itemInfo.push(itemInfo2);
    
    return gameInfoOut.encodeAB();
}