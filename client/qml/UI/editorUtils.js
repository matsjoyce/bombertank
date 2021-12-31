function rectFromDragPoints(startPixelPos, endPixelPos, gridSize, map) {
    var startPos = map.pixelsToPosition(startPixelPos);
    var endPos = map.pixelsToPosition(endPixelPos);

    var startX = Math.min(startPos.x, endPos.x);
    var startY = Math.min(startPos.y, endPos.y);
    var endX = Math.max(startPos.x, endPos.x);
    var endY = Math.max(startPos.y, endPos.y);

    startX = Math.round(startX / gridSize);
    startY = Math.round(startY / gridSize);
    endX = Math.round(endX / gridSize);
    endY = Math.round(endY / gridSize);

    startPixelPos = map.positionToPixels(Qt.point((startX - 0.5) * gridSize, (startY - 0.5) * gridSize));
    endPixelPos = map.positionToPixels(Qt.point((endX + 0.5) * gridSize, (endY + 0.5) * gridSize));

    return {
        startX: startX,
        startY: startY,
        endX: endX,
        endY: endY,
        pixelStart: Qt.point(Math.min(startPixelPos.x, endPixelPos.x), Math.min(startPixelPos.y, endPixelPos.y)),
        pixelEnd: Qt.point(Math.max(startPixelPos.x, endPixelPos.x), Math.max(startPixelPos.y, endPixelPos.y))
    };
}
